# MIDroneControl
OpenViBE BCI paradigm for controlling a drone using 3 classes of Motor Imagery (MI). OpenViBE has been used to handle data flow. 

# Setup and Laboratory Procedure
The following instructions is very specific to the instruments available at the
Hearing Lab at the Technical University of Denmark (DTU) and any replications might need slight adjustments based
on the available equipment.
# Prerequisites
The following instruments and equipment was used.

• 1 x g.USBamp

• 1 x g.GAMMAsys - Preamp

• 16 x g.LADYbird - Active Electrodes

• 1 x g.LADYbirdGND - Active Electrode

• 1 x g.LADYbird - Reference Active Electrode

• 1 x g.BCIgel - Conductive Gel

• 1 x Windows 10 Computer - with min. 2x USB 3.0 ports, OpenViBE installation and driver g.USBamp. Hardware requirements unknown.

• 1 x Drone controlled by ArduPilot software.

• 1 x Taranis Q X7 - Drone Controller

• 1 x Arduino - With a compiled version of the ppm encoder.

# Arduino setup
Everything under ArduinoScript/ppm_encoder_souce must be compiled onto Arduino using the Arduino IDE.
Arduino must output 

# OpenViBE
Connect the g.GAMMAsys to the g.USBamp and the 
g.USBamp to the computer. Turn on everything and start up OpenViBE, and
the OpenViBE acquisition client.

DATA ACQUISITION:

Monitor signal using Mi_Signal_Monitor.xml. When satisfied, use 1Mi_signal_acquisition.mxs to acquire cued data from subject. This is then used in CSP filter training 2Mi_csp_trainer.xml. The CSP filters are applied to 2Mi_classifier_trainer.xml, where the data trains a set of classifiers. The training has now ended.

ONLINE SCRIPTING:

Connect Arduino to computer. Arduino must then be connected to the trainer port of the radio transmitter (The Taranis is equipped with OpenTX) .
OpenViBE 4Mi_online.xml is run using the previously trained filter and classifier configurations. 

5MI_evaluation.xml can be used to evaluate performance and print confusion matrices.



# Credits

Video demonstration of the program
https://www.youtube.com/watch?v=wYRMMQU5Fu8

Part of Bachelors thesis for <b>Carl Emil Elling</b> & <b>Aleksander Sørup Lund</b> in cooperation with DTU Space and DTU Digital Health.

"<i>Exploring the use of Brain-Computer Interfacing in drone control</i>", May 2019

Thesis available through DTU.

Arduino Scripts made by <b>Mikkel Bugge</b> in cooperation with DTU Space
