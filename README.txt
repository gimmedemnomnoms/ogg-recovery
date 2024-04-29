Compilation Instructions:
-------------------------
    To compile this program, use the following command
        gcc recovery.c -o recovery


Run Instructions:
-----------------
    To run this program, you must pass your device name as an argument
    If you are unsure what your device name is, use the command fdisk -l to determine the device. 
    Using the following command, replace "/dev/sda" with your device
        sudo ./recovery /dev/sda

Included Files:
---------------
    tested directory:
        ascensionism.ogg    7.3 mb ogg file used to test the program
        blocks.txt          print out of all blocks containing deleted ogg file
        recovered.ogg       recovered ogg file

