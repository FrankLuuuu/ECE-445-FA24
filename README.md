# Frank Lab Notebook

* [9/8/2024 - First Meeting with Oscar](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#982024---first-meeting-with-oscar)
* [9/9/2024 - In-Person Lab Training](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#992024---in-person-lab-training)
* [9/28/2024 - Voltage Circuit Explanation](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#9282024---voltage-circuit-explanation)
* [10/9/2024 - DIY Arduino PCB](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#1092024---diy-arduino-pcb)
* [10/10/2024 - Power Factor Discussion](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#10102024---power-factor-discussion)
* [10/15/2024 - PCB Meeting with Victor](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#10152024---pcb-meeting-with-victor)
* [10/24/2024 - PCB Training](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#10242024---pcb-training)
* [11/1/2024 - SMD Soldering Training](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#1112024---smd-soldering-training)
* [11/2/2024 - Arduino Soldering](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#1122024---arduino-soldering)
* [11/3/2024 - Finish Soldering](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#1132024---finish-soldering)
* [11/4/2024 - Voltage Testing and PCB Design](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#1142024---voltage-testing-and-redesign)
* [11/8/2024 - More Testing](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#1182024---more-testing)
* [11/9/2024 - Testing and Start Programming](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#1192024---testing-and-start-programming)
* [11/11/2024 - Continue Programming](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#11112024---continue-programming)
* [11/15/2024 - Testing Code]()

# 9/8/2024 - First Meeting with Oscar
first meeting, introduction to the project on a whiteboard, gave us the tolerance level, left the main design component to us, introduced the powernode and how the communication protocal will work, gave us the voltage board, answered any questions we had

# 9/9/2024 - In-Person Lab Training
went to the poets lab for the first time and did the in person high voltage training as well as general lab training, learned about lockout/tagout, got the access to the lab after completing an online training and quiz, we can go to the lab whenever to do our work

# 9/28/2024 - Voltage Circuit Explanation
we held a zoom meeting with oscar for him to explain each component of the voltage circuit to us

# 10/9/2024 - DIY Arduino PCB
we learned after our design review that we need to make our own arduino board instead of using an existing one, researched a lot about arduino designs, did a lot of pcb design on all three boards, decided to do three separate boards instead of a big board due to spacing issues

# 10/10/2024 - Power Factor Discussion
we had a meeting with oscar to find some ideas on how to calculate the power factor with our current design, arduino cannot accept ac signal so have to think of snother way to calculate, we were going to use another chip to calculate all power values, or use another circuit, we decided to do everything in software, we found an article that explains how to calculate the power factor using instantaneous and RMS values

# 10/15/2024 - PCB Meeting with Victor
we help a zoom meeting with an expert on pcb that oscar employs to go over some questions we had about the pcb design including the ground and the voltage switching on the voltage board

# 10/24/2024 - PCB Training
we met with oscar at the poets lab for him to give us some hardware could give us inspiration in the design of the powermeter, taught us some basic soldering techniques and solder removal techniques like using the hot air gun

# 11/1/2024 - SMD Soldering Training
met with oscar again for him to teach us how to use hot plate and soldering paste to solder the smd components we started to solder both the current pcb board and the arduino pcb board

# 11/2/2024 - Arduino Soldering
did more arduino soldering and finished the soldering of the current board

# 11/3/2024 - Finish Soldering
finished soldering the arduino board but realized that we are missing a component that didnt arrive yet, decided to get a component lm317 from the self service center and eceb and solder it the next day

# 11/4/2024 - Voltage Testing and Redesign
finished soldering the arduino and started testing the voltage circuit, the voltage circuit wasnt stepping down the voltage correctly, we think the power source and the signal generator is wrong, we also deciede to output the current value directly from the arduino as an analog 3-phase signal, but the arduino doesn't output analog signals, so we decided to use dac boards and a multiplexer to output the analog signals from the arduino 

# 11/8/2024 - More Testing
started to test the arduino board, the led isnt lighting up when i connect it to power supply, i might have shorted the arduino board by touching two of the pads on the pcb board, did research on how to test the board for connections, did voltage reading tests to debug the board, decided to first write code for the pre-built arduino board and debug using that and then transfer the code on our own arduino board

# 11/9/2024 - Testing and Start Programming
started testing on the current board, started to code for the arduino

# 11/11/2024 - Continue Programming
finished arduino code, current and voltage boards seem to partially work, with some issues

# 11/15/2024 - Testing Code
finished code for both server and client, wrote a separate test script for modular testing of the server, 

