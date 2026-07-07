![Solarium](/Images/Solarium.png)

A simple operating system that as of right now, lets you enter specific commands. As of writing this, the command list is:
- help - Lists commands
- echo - Repeats a string of text
- whoami - Who's using the OS
- about - Info about the OS
- clear - Clears the terminal

This will be updated as time goes on.
CURRENT VERSION: 1.1

## Prerequisites
- Windows Subsystem for Linux (Do `wsl --install` in Powershell, I have Ubuntu installed.)
- [NASM](https://www.nasm.us/https://www.nasm.us/)
- GRUB and xorriso by doing `sudo apt install grub-common xorriso grub-pc-bin grub-efi-amd64-bin` in WSL
- [QEMU](https://www.qemu.org/download/)

## Setup
- Open Visual Studio Code and open the folder.
- Open the terminal in VS Code and **make sure WSL is open, not Powershell.**
- Run the following command:
`make clean && make && make run`

That should be it! Press enter in the GRUB menu and you should see Solarium open!