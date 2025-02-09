#include "ESP8266WiFi.h"
#include <Arduino.h>
#include "WiFiClient.h"
#include <ArduinoJson.h>
#include "pv.h"

#include "apiCtl.h"

#include "config.h"

extern uint64_t pId;
static const char SendDataJson[] PROGMEM = R"EOF(
{
  "model":"%s",
  "id":%u,
  "rssi":%d,
  "ip":"%s",
  "dateTime":"%02u:%02u:%02u %02u/%02u/20%02u",
  "Grid_voltage":%.2f,
  "Grid_frequency":%.2f,
  "Grid_input_power":%d,
  "Grid_charge_current":%.2f,
  "PV_Voltage":%.2f,
  "PV_Charge_current": %.2f,
  "PV_Power":%d,
  "Battery_voltage":%.2f,
  "Battery_capacity":%d,
  "Battery_charge_current":%.2f,
  "Battery_discharge_current":%.2f,
  "Output_voltage":%.2f,
  "Output_frequency":%.2f,
  "Output_active_power":%d,
  "Output_aparent_power":%d,
  "Load_percentage":%.2f,
  "Grid_discharge_today":%.2f,
  "Grid_discharge_total":%.2f,
  "PV_Production_today":%.2f,
  "PV_Production_total":%.2f,
  "Battery_discharge_today":%.2f,
  "Battery_discharge_total":%.2f,
  "Inverter_temperature":%.2f,
  "DC_DC_temperature":%.2f,
  "Error":"%s",
  "Warning":"%s",
  "State":"%s",
  "Charge_priority":"%s",
  "Output_priority":"%s",
  "Grid_rated_voltage":%d,
  "Grid_rated_frequency":%d,
  "Rated_battery_voltage":%d,
  "Max_charge_current":%d,
  "Rated_Output_voltage":%d,
  "Rated_Output_frequency":%d,
  "Rated_Output_power":%.2f,
  "AC_Charge_current":%d,
  "Model": "%s",
  "SerialNumber": "%s",
  "EquipmentMode": "%s",
  "InverterCpuVersion": "%s",
  "MpptCpuVersion": "%s",
  "DateOfManufacture": "%s",
  "ACInMode": "%s",
  "OverLoadRestart": "%s",
  "BatLowToUtiVolt": %.1f,
  "BatteryType": "%s",
  "SysTime": "%d/%d/%d %d:%d:%d",
  "ModBusVer": %.2f,
  "BatLowCutOff": %d,
  "AcToBatVolt": %d,
  "ParaMaxChgCurr": %d,
  "LiProtType": %d,
  "MpptFanSpeed" : %d,
  "InvFanSpeed": %d
})EOF";

void mqttResponseLog(Rtc_Pcf8563 *rtc, char *buf) {
  rtc->getDate();
  rtc->getTime();

  snprintf_P(buf, 1800, SendDataJson,
    "GROWATT SPF5000ES", MODULE_ID, WiFi.RSSI(), WiFi.localIP().toString().c_str(),
    rtc->getHour(), rtc->getMinute(), rtc->getSecond(), rtc->getDay(), rtc->getMonth(), rtc->getYear(),
    Grid_voltage, Grid_frequency, Grid_input_power, Grid_charge_current,
    PV_Voltage, PV_Charge_current, PV_Power,
    Battery_voltage, Battery_capacity, Battery_charge_current, Battery_discharge_current,
    Output_voltage, Output_frequency, Output_active_power, Output_aparent_power, Load_percentage,
    Grid_discharge_today, Grid_discharge_total, PV_Production_today, PV_Production_total, Battery_discharge_today, Battery_discharge_total,
    Inverter_temperature, DC_DC_temperature,
    Error, Warning,
    State, Charge_priority, Output_priority,
    Grid_rated_voltage, Grid_rated_frequency, Rated_battery_voltage,
    Max_charge_current, 
    Rated_Output_voltage, Rated_Output_frequency, Rated_Output_power,
    AC_Charge_current,
    Model, Serial_number, Equipment_mode, Inverter_CPU_version, MPPT_CPU_version, Date_of_manufacture,
    AC_In_Mode, Over_Load_Restart, Bat_Low_To_Uti_Volt, Battery_Type,
    System_Time_Year, System_Time_Month, System_Time_Day, System_Time_Hour, System_Time_Min, System_Time_Sec,
    Mod_Bus_Version, Bat_Low_Cut_Off, Ac_To_BatVolt, Para_Max_Chg_Curr, Li_Protocol_Type,
    Mppt_Fan_Speed, Inv_Fan_Speed
  );
}

