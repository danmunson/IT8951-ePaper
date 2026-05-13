# Notes
- setup instructions [here](https://www.waveshare.com/wiki/7.8inch_e-Paper_HAT#DIP_Switch) - use the bcm2835 libraries
- run the following in the `Raspberry` directory to generate the "push-file" utility

```bash
# build
sudo make -j4 push-file

# run with "sudo ./push-file {VCOM} {FILE_PATH}"
sudo ./push-file -1.46 pic/1872x1404_0.bmp
```


# IT8951-ePaper
## waveshare electronics
![waveshare_logo.png](waveshare_logo.png)
http://www.waveshare.net  
https://www.waveshare.com  

## 中文 ## 
e-Paper IT8951 Driver HAT通用驱动代码，适用于IT8951驱动的并口墨水屏，支持树莓派和STM32

## English ## 
e-Paper IT8951 Driver HAT universal Driver code, suitable for IT8951 Driver parallel ink screen, support Raspberry Pi and STM32
