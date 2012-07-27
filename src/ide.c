/*
 * ide.c -- defines functions relating to IDE drive.
 */

#include "ide.h"
#include "monitor.h"

/* The Command/Status ports returns a bit mask referring to status of
 * a channel when read. Following are its bits. */
#define ATA_SR_BSY  0x80    /* Drive is preparing to send/receive data. */
#define ATA_SR_DRDY 0x40    /* Bit is clear when drive is spun down. */
#define ATA_SR_DF   0x20    /* Drive Fault error. */
#define ATA_SR_DSC  0x10    /* Overlapped Mode Service Request. */ /* TODO */
#define ATA_SR_DRQ  0x08    /* Ready to transfer data. */
#define ATA_SR_CORR 0x04    /* Unused. */
#define ATA_SR_IDX  0x02    /* Unused. */
#define ATA_SR_ERR  0x01    /* Error occurred. */

/* The Features/Error Port has these possible bit masks. */
#define ATA_ER_BBK      0x80
#define ATA_ER_UNC      0x40
#define ATA_ER_MC       0x20
#define ATA_ER_IDNF     0x10
#define ATA_ER_MCR      0x08
#define ATA_ER_ABRT     0x04
#define ATA_ER_TK0NF    0x02
#define ATA_ER_AMNF     0x01

/* When writing to Command/Status port, we will be executing one of the
 * following commands. */
#define ATA_CMD_READ_PIO        0x20
#define ATA_CMD_READ_PIO_EXT    0x24
#define ATA_CMD_READ_DMA        0xC8
#define ATA_CMD_READ_DMA_EXT    0x25
#define ATA_CMD_WRITE_PIO       0x30
#define ATA_CMD_WRITE_PIO_EXT   0x34
#define ATA_CMD_WRITE_DMA       0xCA
#define ATA_CMD_WRITE_DMA_EXT   0x35
#define ATA_CMD_CACHE_FLUSH     0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET          0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY        0xEC

/* The commands for ATAPI devices. */
#define ATAPI_CMD_READ  0xA8
#define ATAPI_CMD_EJECT 0x1B

/* ATA_CMD_IDENTIFY and ATA_CMD_IDENTIFY_PACKET return a buffer of 512 bytes
 * called the identification space. These definitions will be used to extract
 * data from that buffer. */
#define ATA_IDENT_DEVICETYPE    0
#define ATA_IDENT_CYLINDERS     2
#define ATA_IDENT_HEADS         6
#define ATA_IDENT_SECTORS       12
#define ATA_IDENT_SERIAL        20
#define ATA_IDENT_MODEL         54
#define ATA_IDENT_CAPABILITIES  98
#define ATA_IDENT_FIELDVALID    106
#define ATA_IDENT_MAX_LBA       120
#define ATA_IDENT_COMMANDSETS   164
#define ATA_IDENT_MAX_LBA_EXT   200

/* When selecting drives, the interface type and master/slave setting
 * should be specified. */
#define IDE_ATA     0x00
#define IDE_ATAPI   0x01
#define IDE_MASTER  0x00
#define IDE_SLAVE   0x01

/* The Task File is a range of 8 ports which are offset from base address of
 * primary and/or secondary channel. */
#define ATA_REG_DATA        0x00
#define ATA_REG_ERROR       0x01
#define ATA_REG_FEATURES    0x01
#define ATA_REG_SECCOUNT0   0x02
#define ATA_REG_LBA0        0x03
#define ATA_REG_LBA1        0x04
#define ATA_REG_LBA2        0x05
#define ATA_REG_HDDEVSEL    0x06
#define ATA_REG_COMMAND     0x07
#define ATA_REG_STATUS      0x07
#define ATA_REG_SECCOUNT1   0x08
#define ATA_REG_LBA3        0x09
#define ATA_REG_LBA4        0x0A
#define ATA_REG_LBA5        0x0B
#define ATA_REG_CONTROL     0x0C
#define ATA_REG_ALTSTATUS   0x0C
#define ATA_REG_DEVADDRESS  0x0D

#define ATA_PRIMARY     0x00
#define ATA_SECONDARY   0x01
#define ATA_READ        0x00
#define ATA_WRITE       0x01

struct ide_channel_registers {
    /* IO Base. */
    u16int base;
    /* Control Base. */
    u16int ctrl;
    /* Bus Master IDE. */
    u16int bmide;
    /* nIEN (No Interrupt). */
    u8int  nIen;
} channels[2];

/* Buffer to hold the identification space. */
u8int ide_buf[2048] = {0};
/* Variable indicating whether IRQ was invoked. */
static u8int ide_irq_invoked = 0;
/* Array of 6 words (12 bytes) for ATAPI drives. */
static u8int atapi_packet[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

struct ide_device {
    /* 0 (empty) or 1 (this drive exists). */
    u8int reserved;
    /* 0 (Primary channel) or 1 (Secondary Channel). */
    u8int channel;
    /* 0 (Master Drive) or 1 (Slave Drive). */
    u8int drive;
    /* 0 (ATA) or 1 (ATAPI). */
    u16int type;
    /* Drive signature. */
    u16int signature;
    /* Features. */
    u16int capabilities;
    /* Supported Command Sets. */
    u32int commandsets;
    /* Size in sectors. */
    u32int size;
    /* Model as a string. */
    u8int model[41];
} ide_devices[4];

void ide_write(u8int channel, u8int reg, u8int data);

/*
 * Read data from channel and register.
 */
u8int ide_read(u8int channel, u8int reg)
{
    u8int result;
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIen);

    if (reg < 0x08)
        result = inb(channels[channel].base + reg - 0x00);
    else if (reg < 0x0C)
        result = inb(channels[channel].base + reg - 0x06);
    else if (reg < 0x0E)
        result = inb(channels[channel].ctrl + reg - 0x0A);
    else if (reg < 0x16)
        result = inb(channels[channel].bmide + reg - 0x0E);

    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIen);

    return result;
}

/*
 * Write data to a register.
 */
void ide_write(u8int channel, u8int reg, u8int data)
{
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIen);

    if (reg < 0x08)
        outb(channels[channel].base + reg - 0x00, data);
    else if (reg < 0x0C)
        outb(channels[channel].base + reg - 0x06, data);
    else if (reg < 0x0E)
        outb(channels[channel].ctrl + reg - 0x0A, data);
    else if (reg < 0x16)
        outb(channels[channel].bmide + reg - 0x0E, data);

    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIen);
}

#define insl(port, buffer, count) \
    asm volatile ("cld; rep; insl" :: "D"(buffer), "d"(port), "c"(count))


void ide_read_buffer(u8int channel, u8int reg, u32int buffer, u32int quads)
{
    /* TODO: This code contains a serious bug. The inline assembly trashes
     *       ES and ESP for all of the code the compiler generates between
     *       the inline assembly blocks.
     */
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIen);

    asm volatile ("pushw %es; movw %ds, %ax; movw %ax, %es");

    if (reg < 0x08)
        insl(channels[channel].base + reg - 0x00, buffer, quads);
    else if (reg < 0x0C)
        insl(channels[channel].base + reg - 0x06, buffer, quads);
    else if (reg < 0x0E)
        insl(channels[channel].ctrl + reg - 0x0A, buffer, quads);
    else if (reg < 0x16)
        insl(channels[channel].bmide + reg - 0x0E, buffer, quads);

    asm volatile ("popw %es");

    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIen);
}

u8int ide_polling(u8int channel, u32int advanced_check)
{
    /* Delay 400 ns for BSY to be set. */
    int i;
    for (i = 0; i < 4; ++i)
        ide_read(channel, ATA_REG_ALTSTATUS);

    /* Wait for BSY to be cleared. */
    while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY);

    if (advanced_check) {
        /* Read status register. */
        u8int state = ide_read(channel, ATA_REG_STATUS);

        /* Check for errors. */
        if (state & ATA_SR_ERR)
            return 2;

        if (state & ATA_SR_DF)
            return 1;

        if ((state & ATA_SR_DRQ) == 0)
            return 3;
    }

    return 0; /* No error. */
}

u8int ide_print_error(u32int drive, u8int err)
{
    static const char const *PS[] = {"Primary", "Secondary"};
    static const char const *MS[] = {"Master", "Slave"};

    if (err == 0)
        return err;

    monitor_write("IDE: ");
    if (err == 1) {
        monitor_write("- Device Fault");
        err = 19;
    } else if (err == 2) {
        u8int st = ide_read(ide_devices[drive].channel, ATA_REG_ERROR);
        if (st & ATA_ER_AMNF) {
            monitor_write("- No Address Mark Found");
            err = 7;
        }
        if (st & (ATA_ER_TK0NF | ATA_ER_MCR | ATA_ER_MC)) {
            monitor_write("- No Media of Media Error");
            err = 3;
        }
        if (st & ATA_ER_ABRT) {
            monitor_write("- Command Aborted");
            err = 20;
        }
        if (st & ATA_ER_IDNF) {
            monitor_write("- ID Mark Not Found");
            err = 21;
        }
        if (st & ATA_ER_UNC) {
            monitor_write("- Uncorrectable Data Error");
            err = 22;
        }
        if (st & ATA_ER_BBK) {
            monitor_write("- Bad Sectors");
            err = 13;
        }
    } else if (err == 3) {
        monitor_write("- Read Nothing");
        err = 23;
    } else if (err == 4) {
        monitor_write("- Write Protected");
        err = 8;
    }
    monitor_print("\n     - [%s %s] %s\n",
            PS[ide_devices[drive].channel],
            MS[ide_devices[drive].channel],
            ide_devices[drive].model);

    return err;
}

void initialise_ide(u32int bar0, u32int bar1,
        u32int bar2, u32int bar3, u32int bar4)
{
    int i, j, k, count = 0;

    /* Detect IO Ports. */
    channels[ATA_PRIMARY].base      = (bar0 & 0xFFFFFFFC) + 0x1F0 * (!bar0);
    channels[ATA_PRIMARY].ctrl      = (bar1 & 0xFFFFFFFC) + 0x3F4 * (!bar1);
    channels[ATA_SECONDARY].base    = (bar2 & 0xFFFFFFFC) + 0x170 * (!bar2);
    channels[ATA_SECONDARY].ctrl    = (bar3 & 0xFFFFFFFC) + 0x374 * (!bar3);
    channels[ATA_PRIMARY].bmide     = (bar4 & 0xFFFFFFFC) + 0;
    channels[ATA_SECONDARY].bmide   = (bar4 & 0xFFFFFFFC) + 8;

    /* Disable IRQs. */
    ide_write(ATA_PRIMARY,   ATA_REG_CONTROL, 2);
    ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);

    /* Detect ATA-ATAPI devices. */
    for (i = 0; i < 2; ++i) {
        for (j = 0; j < 2; ++j) {
            u8int err = 0, type = IDE_ATA, status;
            /* Assume no drive. */
            ide_devices[count].reserved = 0;

            /* Select drive. */
            ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4));
            /* TODO Wait for 1 ms. */

            /* Send ATA Identify Command. */
            ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
            /* TODO sleep(1); */

            /* Polling.
             * If Status == 0 ==> No device. */
            if (ide_read(i, ATA_REG_STATUS) == 0)
                continue;

            while (1) {
                status = ide_read(i, ATA_REG_STATUS);
                /* If error happend, it is not ATA device. */
                if (status & ATA_SR_ERR) {
                    err = 1;
                    break;
                }
                /* Everything is correct. */
                if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ))
                    break;
            }

            /* Probe for ATAPI Devices. */
            if (err != 0) {
                u8int cl = ide_read(i, ATA_REG_LBA1);
                u8int ch = ide_read(i, ATA_REG_LBA2);

                if ((cl == 0x14 && ch == 0xEB) || (cl == 0x69 && ch == 0x96))
                    type = IDE_ATAPI;
                else
                    continue; /* Unknown type. */
            }

            /* Read Identification Space of the device. */
            ide_read_buffer(i, ATA_REG_DATA, (u32int) ide_buf, 128);

            /* Read device parameters. */
            ide_devices[count].reserved = 1;
            ide_devices[count].type = type;
            ide_devices[count].channel = i;
            ide_devices[count].drive = j;
            ide_devices[count].signature =
                *((u16int *)(ide_buf + ATA_IDENT_DEVICETYPE));
            ide_devices[count].capabilities =
                *((u16int *)(ide_buf + ATA_IDENT_CAPABILITIES));
            ide_devices[count].commandsets =
                *((u32int *)(ide_buf + ATA_IDENT_COMMANDSETS));

            /* Get size. */
            if (ide_devices[count].commandsets & (1 << 26)) {
                /* Device uses 48-bit addressing. */
                ide_devices[count].size =
                    *((u32int *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
            } else {
                /* Device uses CHS or 28 bit addressing. */
                ide_devices[count].size =
                    *((u32int *)(ide_buf + ATA_IDENT_MAX_LBA));
            }

            /* String indicates model of device. */
            for (k = 0; k < 40; k += 2) {
                ide_devices[count].model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
                ide_devices[count].model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];
            }
            ide_devices[count].model[40] = 0;
            count++;
        }
    }

    static const char const *AA[] = { "ATA", "ATAPI" };
    /* Print summary. */
    for (i = 0; i < 4; ++i) {
        if (ide_devices[i].reserved) {
            monitor_print("Found %s drive %u MB - %s\n",
                    AA[ide_devices[i].type],
                    ide_devices[i].size / 1024 / 2,
                    ide_devices[i].model);
        }
    }
}
