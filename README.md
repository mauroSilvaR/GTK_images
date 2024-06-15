# GTK_images
Working with images and button events in gtk

Credits for file_1.png image:

"Solar System" by ShinyOgre licensed CC BY-SA 3.0: [https://opengameart.org/node/3009](https://opengameart.org/content/solar-system)

Credits for example_image_jpg:
Image by Aviavlad on pixabay [https://pixabay.com/pt/photos/electronics-transistor-resistor-3628675/]


# How to compile?
Just open a terminal (CTRL+ALT+T)
go to the folder containing the source code and the example image
and type the following command:

gcc `pkg-config --cflags gtk+-3.0` -o timed_practice timed_practice.c `pkg-config --libs gtk+-3.0`


In order for this command to work, make sure you have GTK installed 
added to the PATH, this is easier when made right at the installation
with sudo apt-get install libgtk-3-dev

after that make sure to run the installation with ./timed_practice

after that just enjoy the program and tweak with it as you please!

Happy learning
