
## Installation
Installation takes a while, as you need to install all tools from ST and then the VSCode extension. This website explains you everything you need to install for the VSCode Extension

https://learn.arm.com/install-guides/stm32_vs/

Details (in case the website is gone). Install the following tools from ST:
- [STM32CubeCLT](https://www.st.com/en/development-tools/stm32cubeclt.html)
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) (not really needed just to avoid errors)
- [ST-MCU-FINDER-PC](https://www.st.com/en/development-tools/st-mcu-finder-pc.html)
- [ST-CORE-AI](https://www.st.com/en/development-tools/stedgeai-core.html#st-get-software) Download it and run ./stedgeai-linux-onlineinstaller. The UI will guide you through the installation process. 

Add the following directory to your PATH variable (e.g., in ~/.bashrc):
- /usr/local/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin
- /opt/ST/STEdgeAI/<Version, e.g., 3.0>/Utilities/linux


Then, install the VSCode extension. 
1. Open the `Extensions` view in Visual Studio Code
2. Search for `STM32 VS Code Extension`
3. Click the `Install` button

Open the project in VSCode. A dialog will pop up in the right lower corner of the window asking to configure the project as an STM32 project. Click yes. The windowd will change with a button called configure. Press enter.
Then, in the same window, type "STM32N6570-DK" under Board / Device and GCC under Toolchain. Add two projects. One called Appli and one called FSBL. Now you should have configured the project.

## Compiling
Prese the Build button in the right lower corner of the window.

## Flashing
Set BOOT1 to H and reset.
Call ./program.sh in the terminal.
Set BOOT0 to L and reset.

## Running the example
Insert and SD card into the board. Open a terminal and log into the serial stream of the board (115200 baudrate). After a while, you should see the message "finished encoding". Insert the SD card into your computer and call (change /dev/sda with the directory of the sd card):

```bash
sudo dd of=dump.bin if=/dev/sda ibs=512 obs=512 count=25000
ffmpeg -f h264 -framerate 30 -i dump.bin -c copy dump.mp4
```

