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
* [11/15/2024 - Testing Code](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#11152024---testing-code)
* [11/17/2024 - Finished Initial Code Testing](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#11172024---finished-initial-code-testing)
* [11/18/2024 - Finished Demo Testing](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#11182024---finished-demo-testing)
* [11/20/2024 - Test DACs and Multiplexer](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#11202024---test-dacs-and-multiplexer)
  


# 9/8/2024 - First Meeting with Oscar
Today marked our first meeting with Oscar, the CEO of PowerBox Technology and our mentor for this project. Oscar introduced us to the Power Meter project, explaining its critical role in ensuring uninterrupted power for industrial factories by providing high-precision real-time power data to the PowerNode. He outlined the core tolerance requirements, emphasizing that voltage and current measurements must achieve an accuracy of ±1%, while real, reactive, and apparent power calculations must fall within a ±1.41% error margin. These strict tolerances adhere to IEC standards and underscore the project’s importance in maintaining operational reliability during grid fluctuations.

Oscar explained the high-level architecture of the PowerBox system, detailing how the Power Meter fits between the factory machinery and the PowerNode. While providing guidance on these foundational aspects, Oscar entrusted the detailed design work to us. This autonomy in designing the current measurement, voltage measurement, and power calculation subsystems challenges us to approach the project with creativity and innovation. During the session, Oscar answered our questions, clarifying both the technical intricacies and the broader industrial applications of the Power Meter.
![image](https://cdn.discordapp.com/attachments/1281703228966436907/1294366232530780160/image.jpg?ex=674a07fd&is=6748b67d&hm=997dddac7a427dafbd56f2af4814d91f67b62318ee9bb87c1c6f8039254a1ec8&)



# 9/9/2024 - In-Person Lab Training
Today, we attended our in-person training session at the POETS lab, where we completed essential training to prepare for working in a high-voltage laboratory environment. The training covered both high-voltage safety procedures and general lab practices, equipping us with the knowledge and skills needed to safely handle the equipment and systems we’ll be working with throughout the project.

A significant portion of the training focused on lockout/tagout (LOTO) procedures, a critical safety practice for ensuring that electrical systems are de-energized and secured before maintenance or modifications. The instructors emphasized the importance of double-checking the lockout status to prevent accidental re-energization, a potentially life-threatening hazard when working with high voltages, which we will be working a lot with for this project.

In addition to the hands-on safety training, we completed an online course and quiz to familiarize ourselves with the lab's protocols. Successfully passing the quiz granted us official access to the POETS lab, allowing us to work independently on our project during lab hours or whenever we need to. This flexibility will be invaluable as we begin prototyping and testing our designs in a controlled and well-equipped environment.

![image](https://github.com/user-attachments/assets/b089796b-dab5-48f8-b47c-96ab68cdd1d4)![image](https://github.com/user-attachments/assets/2782c4e1-29c8-4cb7-8f67-8ec70b9e2529)



# 9/28/2024 - Voltage Circuit Explanation
Today, we had a Zoom meeting with Oscar to dive deeper into the voltage circuit design. He walked us through each component of the circuit, providing detailed explanations of how they work together to measure the RMS voltage of the system. Oscar began by highlighting the importance of the voltage divider circuit, which is crucial for stepping down the high voltage from the inverter to a manageable level that can be processed by the Arduino. He explained how the resistors in the divider circuit are selected to ensure that the voltage is within the Arduino’s input range of 0-5V.

Oscar also introduced us to the AD737 chip, which is used to convert the RMS voltage into a DC voltage. He emphasized that the use of DC voltage is beneficial for maintaining a stable measurement, which is essential for accurate power calculations. 

Additionally, Oscar answered questions about the current transformers, its role in the whole design, and how the voltage and current measurement subsystems will interact. This meeting helped clarify the functional relationships between the components and gave us a clearer understanding of how the circuit is designed to meet the project’s accuracy requirements.



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
finished code for both server and client, wrote a separate test script for modular testing of the server with arbitrary current value/ratio, voltage value/ratio, and power factor, started testing the arduino code, found compiler errors saying modbus tcp isn't compatible with arduino uno, will make changes to the code


# 11/17/2024 - Finished Initial Code Testing
revised the server and client code to be compatible with arduino uno by using the arduino ethernet library to simulate the modbus protocol, server now listens to requests for the data and responds with the calculated data, the client side sends the requests in a loop and outputs the calculated power data


# 11/18/2024 - Finished Demo Testing
adjusted the server code to read ac current and voltage from the arduino pins, adjusted the readings with dc offsets and calibrations, power data calculated via sampling and getting the RMS value 


# 11/20/2024 - Test DACs and Multiplexer
tested the DAC output with hardcode values and the multiplexer selecting which DAC to output the value
