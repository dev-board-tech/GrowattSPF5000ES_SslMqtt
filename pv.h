#ifndef __PV_H__
#define __PV_H__

#define DEVICE_ID	1

#define MAIN_PAGE_BUFFER_LEN  1

extern double Grid_voltage;
extern double Grid_frequency;
extern int Grid_input_power;
extern double Grid_charge_current;
extern double PV_Voltage;
extern double PV_Charge_current;
extern int PV_Power;
extern double Battery_voltage;
extern int Battery_capacity;
extern double Battery_charge_current;
extern double Battery_discharge_current;
extern double Output_voltage;
extern double Output_frequency;
extern int Output_active_power;
extern int Output_aparent_power;
extern double Load_percentage;
extern double Grid_discharge_today;
extern double Grid_discharge_total;
extern double PV_Production_today;
extern double PV_Production_total;
extern double Battery_discharge_today;
extern double Battery_discharge_total;
extern double Inverter_temperature;
extern double DC_DC_temperature;
extern char Error[];
extern char Warning[];
extern char State[];
extern char Charge_priority[];
extern char Output_priority[];
extern int Grid_rated_voltage;
extern int Grid_rated_frequency;
extern int Rated_battery_voltage;
extern int Max_charge_current;
extern int Rated_Output_voltage;
extern int Rated_Output_frequency;
extern double Rated_Output_power;
extern int AC_Charge_current;
extern double Bat_Low_To_Uti_Volt;
extern uint16_t System_Time_Year;
extern uint8_t System_Time_Month;
extern uint8_t System_Time_Day;
extern uint8_t System_Time_Hour;
extern uint8_t System_Time_Min;
extern uint8_t System_Time_Sec;
extern double Mod_Bus_Version;
extern uint16_t Bat_Low_Cut_Off;
extern uint16_t Ac_To_BatVolt;
extern uint16_t Para_Max_Chg_Curr;
extern uint16_t Li_Protocol_Type;
extern uint8_t Mppt_Fan_Speed;
extern uint8_t Inv_Fan_Speed;


extern char Model[];
extern char Serial_number[];
extern char Equipment_mode[];
extern char Inverter_CPU_version[];
extern char MPPT_CPU_version[];
extern char Date_of_manufacture[];
extern char AC_In_Mode[];
extern char Over_Load_Restart[];
extern char Battery_Type[];


extern int BatteryPower;
extern char PowerSourceStatus[];
extern double PV_PowerLoad;
extern double Battery_PowerLoad;

extern uint32_t faultCode;
extern uint32_t warningCode;
extern uint16_t systemStatus;
extern uint8_t chargePriority;


void PV_Init();
void PV_Loop();
void PV_triggerDataRead();
long PV_getDataFefreshCnt();
unsigned short *PV_getHoldingReg();
unsigned short *PV_getInputReg();
void PV_WriteData(char devId, short addr, short data);
bool PV_GetBusy();

#endif