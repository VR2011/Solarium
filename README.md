![Solarium](/Images/Solarium.png)
Solarium is a simple operating system that, as of right now, lets you enter some commands.

## Setup
Install the ISO from releases and put it in your favorite VM!

That should be it! Press Enter in the GRUB menu and you should see Solarium open!

Don't want to use a VM? That's alright! You can go [here](https://copy.sh/x86)! Scroll down to setup, put the ISO in CD Image, and load it there by clicking Start Emulation!

## Building
This isn't needed to install the OS! Use the instructions above instead.

**PREREQUISITES**
If you're on Windows, install Windows Subsystem for Linux first by running `wsl --install` in an Administrator PowerShell. Make sure to use Ubuntu as the distro; but it should already be that as default.

You'll then need to install these tools.
- **[NASM](https://www.nasm.us/)**
- Run this command inside of WSL. It will install packages the OS needs for it to work.
  ```
  sudo apt install grub-common grub-pc-bin grub-efi-amd64-bin xorriso build-essential gcc-multilib make
  ```
- **[QEMU](https://www.qemu.org/download/)**
- You'll also need Python 3 and Pillow.
  ```
  sudo apt install python3 python3-pip
  pip3 install pillow --break-system-packages
  ```
  Building the OS should work on other operating systems with different steps, but this is my setup as I use Windows.
**BUILDING**
In the Github repo, click Code and then Download ZIP. Once done, extract the zip file. You should see a folder which has all the files.

Once everything above is done, open the folder in an IDE (Preferably VS Code) and open a WSL window. In there, run this command:
```
make clean && make && make run
```

You should now see a new window that shows the OS menu! You can now customize the OS as you'd like. Once you finish, save all files and re-run the command above.
