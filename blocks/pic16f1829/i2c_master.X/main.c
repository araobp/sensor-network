#include "mcc_generated_files/mcc.h"
#include "protocol.h"
#include "i2c1_util.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEVICE_ID "BACKPLANE-MASTER"

// Backplane-master-specific commands
#define I2C "I2C"
#define MAP "MAP"
#define POS "POS"
#define WSC "WSC"
#define RSC "RSC"
#define CSC "CSC"
#define SSC "SSC"
#define EDG "EDG"
#define SEN "SEN"
#define SFT "SFT"
#define UFT "UFT"

// I2C slave device address range handled by backplane master
// These constants should be larger than 16 and multipels of 8
#define MIN_DEV_ADDR 0x10
#define MAX_DEV_ADDR 0x20

#define MASK 0x01

#define INV_DELAY 200 // INV delay (micro sec)
#define INV_SEN_DELAY 1000  // INV->SEN delay (micro sec))
#define SEN_DELAY 100   // SEN delay (micro sec)

const uint8_t MAX_Y = MAX_DEV_ADDR/8;

char cmd_buf[4][BUF_SIZE];

 /*
 * Schedule with I2C slave addresses
 * 
 * 0: every 8msec
 * 1: every 16 msec (8 * 2)
 * 2: every 48 msec (8 * 6): ~50msec
 * 3: every 96 msec (8 * 12): ~100msec
 * 4: every 480 msec (8 * 60): ~500msec
 * 5: every 960 msec (8 * 120): ~1sec
 * 6: every 4800 msec (8 * 600): ~5sec
 */
uint8_t schedule[7][4];

uint8_t timer_cnt = 0;
bool do_func = false;
uint8_t read_buf[16];
uint8_t dev_map[MAX_Y];  // I2C slave device map

bool filter = true;  // filter out NO_DATA

bool exec_remote_cmd(uint8_t idx) {
    uint8_t data;
    uint8_t i, j;
    uint8_t status;

    uint8_t send_buf[BUF_SIZE + 2];

    char *buf = cmd_buf[idx];
    if (buf[0] == '\0') return false;
        
    if (parse(WHO, buf)) {
        status = i2c1_read(BACKPLANE_SLAVE_ADDRESS, WHO_I2C, &data, 1);
        if (status == 0) printf("$:WHO:%d\n", data);
        else printf("!:WHO:NACK\n");        
    } else if (parse(SAV, buf)) {
        status = i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, SAV_I2C);
    } else if (parse(STA, buf)) {
        status = i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, STA_I2C);
    } else if (parse(STP, buf)) {
        status = i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, STP_I2C);
        if (status == 0) printf("*:STP:ACK\n");
        else printf("!:STP:NACK\n");
    } else if (parse(SET, buf)) {
        data = atoi(&buf[4]);
        i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, SET_I2C);
        i2c1_write_no_data(BACKPLANE_SLAVE_ADDRESS, data);        
    } else if (parse(GET, buf)) {
        i2c1_read(BACKPLANE_SLAVE_ADDRESS, GET_I2C, &data, 1);
        printf("$:GET:%d\n", data);
    /* Extended commands */
    } else {
        i = 0;
        j = 2;
        do {
            send_buf[j++] = (uint8_t)buf[i];
        } while (buf[i++] != '\0');
        send_buf[0] = EXT_I2C;
        send_buf[1] = i;
        status = i2c1_write(BACKPLANE_SLAVE_ADDRESS, send_buf, j);
        if (status != 0) printf("!:NO ACK FROM I2C SLAVE\n");
        // printf("\n");
    }
    buf[0] = '\0';
    return true;
}

void init(void) {
    uint8_t y, i, j, k, dev_addr;

    // clear dev map
    for (y=0; y<MAX_Y; y++) {
        dev_map[y] = 0;
    }

    // clear cmd buf
    for (i=0; i<4; i++) cmd_buf[i][0] = '\0';    

    // initialize schedule
    for (i=0; i<28; i++) {
        dev_addr = DATAEE_ReadByte(SCHEDULE_ADDRESS+i);
        schedule[i/4][i%4] = dev_addr;
    }
}

uint8_t devs = 0;
        
void add_dev(uint8_t dev_addr) {
    uint8_t x, y;
    uint8_t new_map, old_map;
    if (dev_addr >= 1 && dev_addr <= MAX_DEV_ADDR) {
        y = dev_addr / 8;
        x = dev_addr % 8;
        old_map = dev_map[y];
        new_map = dev_map[y] | (0x01 << x);
        if (old_map != new_map) {
            dev_map[y] = new_map;
            devs++;
        }
    }
}

void del_dev(uint8_t dev_addr) {
    uint8_t x, y;
    uint8_t new_map, old_map;
    if (dev_addr >= 1 && dev_addr <= MAX_DEV_ADDR) {
        y = dev_addr / 8;
        x = dev_addr % 8;
        old_map = dev_map[y];
        new_map = dev_map[y] & ~(0x01 << x);
        if (old_map != new_map) {
            dev_map[y] = new_map;
            devs--;
        }
   }
}

/*
 * I2C slave device map iterator
 */
uint8_t dev_map_iterator() {
    static uint8_t xx = 0;
    static uint8_t yy = 0;
    static bool start = true;
    bool exist = false;
    uint8_t dev_addr;

    if (devs) {
        while(1) {
            if (xx > 7) {
                xx = 0;
                yy++;
            }
            if (yy >= MAX_Y) {
                xx = 0;
                yy = 0;
            }
            if ((MASK << xx) & dev_map[yy]) {
                dev_addr = yy * 8 + xx++;
                break;
            }
            xx++;
        }
    }
    return dev_addr;
}

/*
 * scan I2C slave devices
 */
void scan_dev(void) {
    uint8_t dev_addr, status;
    for (dev_addr=MIN_DEV_ADDR; dev_addr<=MAX_DEV_ADDR; dev_addr++) {
        status = i2c1_read(dev_addr, WHO_I2C, &read_buf[0], 1);
        // printf("%d %d\n", dev_addr, status);
        if (status == 0 && dev_addr == read_buf[0]) {
            add_dev(dev_addr);
        } else {
            del_dev(dev_addr);
        }
    }  
}

/*
 * detect new I2C slave device
 */
void check_plg(void) {
    uint8_t status = i2c1_write_no_data(GENERAL_CALL_ADDRESS, PLG_I2C);
    if (status == 0) scan_dev();
}

/*
 * check if the I2C slave device has already been detected
 */
bool detected(uint8_t dev_addr) {
    uint8_t y = dev_addr / 8;
    uint8_t x = dev_addr % 8;
    bool exist = false;
    if ((dev_map[y] & (0x01 << x)) > 0) {
        exist = true;
    }
    return exist;
}

void print_dev_map(void) {
    uint8_t i;
    uint8_t len = devs;
    if (len > 0) {
        len--;
        printf("$:MAP:");
        for (i=0; i<len; i++) printf("%d,", dev_map_iterator());
        printf("%d\n", dev_map_iterator());
    } else {
        printf("!:MAP:NO SLAVE FOUND\n");
    }
}


/*
 * retrieve sensor data from I2C slave device
 */
uint8_t sen(uint8_t dev_addr) {
    uint8_t status;
    uint8_t type;
    uint8_t length ,data, i;
    LED_RED ^= 1;
    // INV
    __delay_us(INV_DELAY);
    i2c1_write_no_data(dev_addr, INV_I2C);
    // SEN
    __delay_us(INV_SEN_DELAY);
    status = i2c1_read(dev_addr, SEN_I2C, &type, 1);
    __delay_us(SEN_DELAY);
    if (status == 0) {
        if (type == TYPE_NO_DATA) {
            if (!filter) {
                PROTOCOL_Print_TLV(dev_addr, TYPE_NO_DATA, 0, NULL);            
            }
        } else {
            status = i2c1_read(dev_addr, SEN_I2C, &length, 1);
            __delay_us(SEN_DELAY);
            if (status == 0) {
                status = i2c1_read(dev_addr, SEN_I2C, read_buf, length);
                __delay_us(SEN_DELAY);
                if (status == 0) {
                    PROTOCOL_Print_TLV(dev_addr, type, length, read_buf);
                }
            }
        }                    
    }   
    return status;
}

/*
 * one-shot INV/SEN
 */
void one_shot_sen(uint8_t dev_addr) {
    if (!detected(dev_addr)) {
        printf("!:%%%d:UNDETECTED DEVICE\n", dev_addr);
    } else {
        uint8_t status = sen(dev_addr);
        if (status > 0) {
            printf("!:%%%d:NETWORK ERROR\n", dev_addr);
            i2c1_write_no_data(dev_addr, RST_I2C); 
        }
    }
}
        
/*
 * invoke and retrive sensor data
 */
void fetch(uint8_t *sch) {
    uint8_t pos;
    uint8_t dev_addr;
    uint8_t status;
    for (pos=0; pos<4; pos++) {
        dev_addr = sch[pos];
        if (dev_addr == 0 || dev_addr > MAX_DEV_ADDR) {
            break;
        } else if (detected(dev_addr)) {
            status = sen(dev_addr);
            if (status > 0) {
                printf("!:%%%d:NETWORK ERROR\n", dev_addr);
                i2c1_write_no_data(dev_addr, RST_I2C); 
            }
        }
    }
}

/*
 * periodic task (tick: 8msec)
 * (every 8 * 60 msec):
 *   - detect a new device with I2C general call
 *   - execute a remote command
 * (every 8 * 600 msec):
 *   - scan devices
 */
void tick_handler(void) {
    static uint16_t t = 0;
    static int8_t cmd_next = 0;
    t++;

    if (exec_remote_cmd(cmd_next)) {
        cmd_next++;
        if (cmd_next > 3) cmd_next = 0;
    }

    if (t % 60 == 0) {
        check_plg();
    } else if (t % 600 == 0) {
        scan_dev();
        t = 0;
    }
}

/*
 * scheduler: periodic tasks
 */
void inv_handler(void) {
    uint8_t dev_addr, status;
    static uint16_t t = 0;
    
    /*** 8msec ***/
    fetch(schedule[0]);
    /*** 16msec ***/
    if (t % 2 == 0) fetch(schedule[1]);
    /*** 48msec (~50msec) ***/
    if (t % 6 == 0) fetch(schedule[2]);
    /*** 96msec (~100msec)***/
    if (t % 12 == 0) fetch(schedule[3]);
    /*** 480msec (~500msec) ***/
    if (t % 60 == 0) fetch(schedule[4]);
    /*** 960msec (~1sec) ***/
    if (t % 120 == 0) fetch(schedule[5]);
    /*** 4800msec (~5sec) ***/
    if (t % 600 == 0) {
        fetch(schedule[6]);
        t = 1;
    }
    /*** count up schedule timer */
    t++;
}

/*
 * put command into buffer to execute it on I2C slave device
 */
void put_cmd(uint8_t *buf) {
    static int8_t idx = -1;
    if (++idx > 3) idx = 0;
    if (cmd_buf[idx][0] == '\0') strcpy(cmd_buf[idx], buf);
}

/*
 * Backplane-master-specific commands
 */
void command_handler(uint8_t *buf) {
    static uint8_t pos = 0;
    uint8_t i;
    uint8_t dev_addr;
    if (parse(I2C, buf)) {
        BACKPLANE_SLAVE_ADDRESS = atoi(&buf[4]);
    } else if (parse(MAP, buf)) {
        print_dev_map();
    } else if (parse(POS, buf)) {
        pos = atoi(&buf[4]);
        if (pos > 27) printf("!:POS:POS LARGER THAN 27\n");
    } else if (parse(WSC, buf)) {
        if (pos <= 27) {
            dev_addr = atoi(&buf[4]);
            schedule[pos/4][pos%4] = dev_addr;
        } else {
            printf("!:WSC:POS LARGER THAN 27\n");
        }
    } else if (parse(SSC, buf)) {
        for(i=0; i<28; i++) {
            dev_addr = schedule[i/4][i%4];
            DATAEE_WriteByte(SCHEDULE_ADDRESS + i, dev_addr);
        }
    } else if (parse(RSC, buf)) {
        printf("$:RSC:");
        for (i=0; i<27; i++) {
            if ((i+1) % 4 == 0) {
                printf("%d|", schedule[i/4][i%4]);
            } else {
                printf("%d,", schedule[i/4][i%4]);
            }
        }
        printf("%d\n", schedule[6][3]);
    } else if (parse(CSC, buf)) {
        for (i=0; i<28; i++) {
            schedule[i/4][i%4] = 0;
        }
    } else if (parse(SEN, buf)) {
        dev_addr = atoi(&buf[4]);
        one_shot_sen(dev_addr);
    } else if (parse(SFT, buf)) {
        filter = true;
    } else if (parse(UFT, buf)) {
        filter = false;
    } else if (BACKPLANE_SLAVE_ADDRESS != BACKPLANE_MASTER_I2C) {
        put_cmd(buf);
    }
}

void main(void)
{
    init();

    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    PROTOCOL_Initialize(DEVICE_ID, NULL, NULL, NULL, inv_handler, 1);
    PROTOCOL_Set_Extension_Handler(command_handler);
    PROTOCOL_Set_Tick_Handler(tick_handler);
    PROTOCOL_Loop();
}
