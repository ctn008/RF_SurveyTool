gnss_lib_py# Reading data from GPS device  TW5262

## 1. Physical Interface & Connectivity  
TW5262 uses uBlox M8 with UDR Untethered Dead Rekoning technology.
- TW5262 comes with RJ12 connector with 06 pins, whereas 05 pins are used (Tx, Rx, Gnd, Pwr+, Ignite ON/OFF)
- Use mini-usb serial port converter to convert usb port into serial port, which have DB9 male pins.
- To connect DB9 plug to RJ12, a RJ12 socket is used to get jumper outs.

**Issues encountered in establishing physical connection**  
- physical contact of wiring. Initially no signal received. Need an osciloscope to capture the signal ?
- serial port has no power source, even though pin 7 and pin 4 can provide 10vdc, but tried to use this power source unsuccessfully.
- use external 9v battery source with common ground. After checking wiring contacts properly, the signals are active (software LED blinking)
- baud rate: in tw5262 manual shows 9600,8,N,1 as default, but actually it is at 115200 baudrate.
- Using uBlox u-center software tool to capture the signal from tw5262. And use Automated Serial Terminal software to read. Initially reading at 9600 baud, only Serial Terminal can display text outputs (but uBlox u-center should be able to display text console similarly). After changing to auto baud rate, u-center was able to receive the signal at 115200 bps.

**Learning**  
Using computer soundcard as osciloscope. There is  a commercial software digilent waveforms that can turn a soundcard to be a hardware input. And there is a hardware box with usb connection at $300 range.  

Also there is a very cheap antenna vector network analyzer ($105 usd ???)  

Calibrate rtlsdr device usign Kalibrate software - tried unsuccessfully.  

## 2. Reading serial port    
Need to understand how often the data is updated, and data contents.
It seems that tw5262 update rate (default) is 1s. The data would be multiple line.
One issue being observed is that the read data being corrupted. i.e. by theory, every NMEA message should start with $, but some readings don't have this $ (using python serial read)

**NOTE**: reading gps data from usb-serial port in Windows yields good data. But in linux, the data was lost / corrupted. Probably due to linux driver issues - not yet able to investigate.  

# GNSS Libraries for reference  
gnss_lib_py developped by Stanford seems to be a good library.  

GGA 	Time, position, and fix related data
$GNGGA,080913.00,3725.77992,N,12208.00141,W,2,12,0.54,27.3,M,-30.0,M,,0000*46

GLL 	Position data: position fix, time of position fix, and status
$GNGLL,3725.77992,N,12208.00135,W,080912.00,A,D*6D

RMC 	Position, Velocity, and Time
$GNRMC,080913.00,A,3725.77992,N,12208.00141,W,0.011,,131224,12.59,E,D,V*6F
      This message includes time and date info.
