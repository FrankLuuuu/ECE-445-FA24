# Frank Lab Notebook

* [9/8/2024 - First Meeting with Oscar](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#982024---first-meeting-with-oscar)
* [9/9/2024 - In-Person Lab Training](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#992024---in-person-lab-training)
* [9/28/2024 - Voltage Circuit Explanation](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#9282024---voltage-circuit-explanation)
* [10/9/2024 - DIY Arduino PCB](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#1092024---diy-arduino-pcb)
* [10/10/2024 - Power Factor Discussion](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#10102024---power-factor-discussion)
* [10/15/2024 - PCB Meeting with Victor](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#10152024---pcb-meeting-with-victor)
* [10/24/2024 - PCB Training](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#10242024---pcb-training)
* [11/4/2024 - Voltage Testing and PCB Design](https://github.com/FrankLuuuu/ECE-445-FA24/tree/main?tab=readme-ov-file#1142024---voltage-testing-and-redesign)
* [11/8/2024 - More Testing](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#1182024---more-testing)
* [11/15/2024 - Testing Code](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#11152024---testing-code)
* [11/17/2024 - Finished Initial Code Testing](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#11172024---finished-initial-code-testing)
* [11/18/2024 - Finished Demo Testing](https://github.com/FrankLuuuu/ECE-445-FA24/blob/main/README.md#11182024---finished-demo-testing)
  


# 9/8/2024 - First Meeting with Oscar
Today marked our first meeting with Oscar, the CEO of PowerBox Technology and our mentor for this project. Oscar introduced us to the Power Meter project, explaining its critical role in ensuring uninterrupted power for industrial factories by providing high-precision real-time power data to the PowerNode. He outlined the core tolerance requirements, emphasizing that voltage and current measurements must achieve an accuracy of ±1%, while real, reactive, and apparent power calculations must fall within a ±1.41% error margin. These strict tolerances adhere to IEC standards and underscore the project’s importance in maintaining operational reliability during grid fluctuations.

Oscar explained the high-level architecture of the PowerBox system, detailing how the Power Meter fits between the factory machinery and the PowerNode. While providing guidance on these foundational aspects, Oscar entrusted the detailed design work to us. This autonomy in designing the current measurement, voltage measurement, and power calculation subsystems challenges us to approach the project with creativity and innovation. During the session, Oscar answered our questions, clarifying both the technical intricacies and the broader industrial applications of the Power Meter.
<img width="919" alt="Screenshot 2024-12-01 at 5 15 34 PM" src="https://github.com/user-attachments/assets/5fe65b15-f69c-481e-a8c9-e285d623b92c">




# 9/9/2024 - In-Person Lab Training
Today, we attended our in-person training session at the POETS lab, where we completed essential training to prepare for working in a high-voltage laboratory environment. The training covered both high-voltage safety procedures and general lab practices, equipping us with the knowledge and skills needed to safely handle the equipment and systems we’ll be working with throughout the project.

A significant portion of the training focused on lockout/tagout (LOTO) procedures, a critical safety practice for ensuring that electrical systems are de-energized and secured before maintenance or modifications. The instructors emphasized the importance of double-checking the lockout status to prevent accidental re-energization, a potentially life-threatening hazard when working with high voltages, which we will be working a lot with for this project.

In addition to the hands-on safety training, we completed an online course and quiz to familiarize ourselves with the lab's protocols. Successfully passing the quiz granted us official access to the POETS lab, allowing us to work independently on our project during lab hours or whenever we need to. This flexibility will be invaluable as we begin prototyping and testing our designs in a controlled and well-equipped environment.

![image](https://github.com/user-attachments/assets/b089796b-dab5-48f8-b47c-96ab68cdd1d4)![image](https://github.com/user-attachments/assets/2782c4e1-29c8-4cb7-8f67-8ec70b9e2529)



# 9/28/2024 - Voltage Circuit Explanation
Today, we had a Zoom meeting with Oscar to dive deeper into the voltage circuit design. He walked us through each component of the circuit, providing detailed explanations of how they work together to measure the RMS voltage of the system. Oscar began by highlighting the importance of the voltage divider circuit, which is crucial for stepping down the high voltage from the inverter to a manageable level that can be processed by the Arduino. He explained how the resistors in the divider circuit are selected to ensure that the voltage is within the Arduino’s input range of 0-5V.

Oscar also introduced us to the AD737 chip, which is used to convert the RMS voltage into a DC voltage. He emphasized that the use of DC voltage is beneficial for maintaining a stable measurement, which is essential for accurate power calculations. 

Additionally, Oscar answered questions about the current transformers, their role in the whole design, and how the voltage and current measurement subsystems will interact. This meeting helped clarify the functional relationships between the components and gave us a clearer understanding of how the circuit is designed to meet the project’s accuracy requirements.



# 10/9/2024 - DIY Arduino PCB
After our design review, we learned that we need to create our own custom Arduino board instead of using an existing one. To prepare for this, we spent the day researching various Arduino designs and diving into the intricacies of PCB design. We examined different layouts and considerations, including power distribution, signal integrity, and component placement, to ensure our custom board would meet the high-level requirements.

![Screenshot 2024-11-29 at 8 34 43 AM](https://github.com/user-attachments/assets/34d52b9f-bf84-4cff-ad25-12124ac25423)

In the process, we also worked on the PCB design for all three major subsystems: the current measurement circuit, voltage measurement circuit, and power calculation subsystem. After evaluating our options and the available space, we decided to go with three separate PCB boards instead of a single large board. This approach was chosen to mitigate spacing issues and ensure that each subsystem could be optimized independently, making the overall design more manageable and scalable.

The day was a productive step toward creating the PCBs, and we now have a clearer direction for the layout and design of the individual boards.



# 10/10/2024 - Power Factor Discussion
Today, we had a meeting with Oscar to discuss potential solutions for calculating the power factor within our current design. One challenge we encountered is that the Arduino cannot directly handle AC signals, which is a critical aspect for calculating power factor. Initially, we considered using another chip to handle the power calculations, or possibly incorporating an additional circuit to calculate all the power values. However, after further discussion and exploration, we decided to tackle the power factor calculation entirely in software.

We researched various methods for calculating power factor, and found an article that provided a detailed explanation on how to compute it using both instantaneous and RMS voltage and current values. This approach involves sampling the voltage and current at high frequencies, calculating their instantaneous product, and then averaging over a set of samples to obtain the real power. The apparent power can be derived from the RMS values of voltage and current. The power factor is then simply the ratio of real power to apparent power.

This solution aligns well with our project’s constraints and simplifies the design by avoiding the need for additional hardware. With this approach in mind, we are now able to proceed with the software implementation for the power factor calculation. Here is the main source that we used: https://docs.openenergymonitor.org/electricity-monitoring/ac-power-theory/arduino-maths.html.



# 10/15/2024 - PCB Meeting with Victor
We held a Zoom meeting with Victor, an expert in PCB design who works with Oscar, to go over some specific questions we had regarding our PCB designs. Victor was brought in to help us address some of the technical challenges we encountered while designing the boards for the Power Meter project.

During the meeting, we focused on two main areas of concern: the grounding strategy and voltage switching on the voltage board. Victor provided valuable insights into proper grounding techniques, emphasizing the importance of minimizing noise and interference, especially since we are working with high-precision voltage and current measurements. He also clarified how to handle voltage switching on the voltage board to ensure stability and accuracy, particularly when dealing with the stepped-down voltage signals that will be processed by the Arduino.



# 10/24/2024 - Soldering Training
Today, we met with Oscar at the POETS lab, where he provided us with some hardware that could serve as inspiration for the design of our Power Meter. This was a valuable opportunity to see how other components are built and to gather ideas for refining our own design. During the session, Oscar also taught us several essential soldering techniques, which will be crucial as we begin assembling our PCBs.

Oscar walked us through basic soldering methods, including how to properly solder through-hole components and surface-mount devices (SMD). He then demonstrated techniques like using a hot air gun for solder removal and how to work with a hot plate and soldering paste for soldering SMD components. This was especially helpful for handling the small and delicate components on our custom Arduino and current measurement PCB boards.

With these new skills, we started soldering both the current measurement PCB board and the Arduino PCB board. 



# 11/4/2024 - Voltage Testing and Redesign
Today, we completed the soldering of the custom Arduino PCB and moved forward with testing the voltage circuit. During testing, we discovered that the voltage circuit was not stepping down the voltage as expected. After further investigation, we concluded that the issue might be related to the power source and the signal generator we were using. We plan to recheck both the power source and the signal generator to ensure they are providing the correct inputs for the voltage circuit.

In addition to the voltage circuit issues, we also made some key decisions regarding the output of the current values. Initially, we had planned to output the current values directly from the Arduino as a 3-phase analog signal. However, since the Arduino does not natively support analog outputs, we pivoted and decided to use DAC boards and a multiplexer to generate and output the analog signals. This will allow us to send the current values as analog signals for further processing, ensuring that the system operates as intended.

The DACs will be implemented with the help of this tutorial: https://learn.adafruit.com/mcp4725-12-bit-dac-tutorial?view=all.



# 11/8/2024 - More Testing
Today, we began testing the custom Arduino board, but ran into an issue: the LED wasn’t lighting up when connected to the power supply. After some troubleshooting, we suspect that the board might have been shorted during assembly, possibly from accidentally touching two of the pads on the PCB. To verify this, I did some research on how to test the board for short circuits and potential connection issues.

I performed voltage reading tests at various points on the board to debug the connections and check for irregularities. These tests helped us identify areas where the board might not be receiving the correct voltage, which could explain the lack of functionality. I realized that the 5V regulator was not outputting the correct voltage values, which could indicate a faulty component that will need to be replaced.

Given the complications with the custom Arduino board, I decided to shift the testing strategy. I wrote and tested the code on a pre-built Arduino board first, using it as a reliable platform for debugging. Once the code works as expected on the pre-built board, I’ll transfer it to the custom Arduino board to continue development. This will help ensure that the logic is correct before dealing with any potential hardware issues on the custom board.



# 11/15/2024 - Testing Code
Today, I completed the Arduino code for both the server and client components of the Power Meter system. To ensure that each part of the system functions correctly, I also wrote a separate test script for modular testing. The script allows us to simulate arbitrary values for current, voltage, and power factor, providing a way to validate the server’s behavior with a variety of input scenarios.

When I began testing the code on the Arduino, I encountered some compiler errors related to the Modbus TCP library. The error indicated that Modbus TCP is not compatible with the Arduino Uno, which uses limited memory and processing power compared to more advanced Arduino models. This compatibility issue is something that will need to be addressed before proceeding further.

To resolve this, I’ll need to modify the code, either by finding a compatible library for the Arduino Uno or by considering an alternative approach to implementing the Modbus TCP communication protocol. These changes will ensure that the system can function as intended with the available hardware.

<img width="1101" alt="image" src="https://github.com/user-attachments/assets/6e79d6f1-e6b9-4a2c-bdf2-cb7f0bd90db8">




# 11/17/2024 - Finished Code Testing
I revised the server and client code to make it compatible with the Arduino Uno. Since the Modbus TCP library was not compatible with the Arduino Uno due to memory limitations, I switched to using the Arduino Ethernet library to simulate the Modbus protocol. This adjustment allowed us to implement basic communication without requiring additional hardware or modifications to the existing Arduino Uno setup.

With the new configuration, the server is now able to listen for incoming requests and respond with the calculated power data, including the current, voltage, and power factor values. On the client side, the code sends requests in a continuous loop to the server, which outputs the calculated power data in response.

The communication between the server and client is now functioning as expected, and the system is able to simulate the power calculations and transmit the data correctly to a tolerance of around 1%. This marks a significant step in verifying the system's functionality before moving on to integration with the other systems.

<img width="779" alt="Screenshot 2024-11-29 at 12 06 37 PM" src="https://github.com/user-attachments/assets/2441bff8-0a76-4cbf-8851-a7c0b34aacdf">



# 11/18/2024 - Finished Demo Testing
Today, I made several adjustments to the server code to integrate the AC current and voltage readings from the Arduino’s pins. I modified the code to handle the AC signals by applying DC offsets and calibrating the readings for more accurate voltage and current measurements. The power data is now calculated through sampling and the computation of the RMS values.

I also tested the DAC output by using hardcoded values to simulate the current values. This allowed me to verify that the DAC board was functioning correctly and that the multiplexer was properly selecting which DAC to output the value from. This setup is crucial for providing the analog output signals corresponding to the measured current and voltage, which will be used in further calculations.

The system should be now fully functional for demo purposes, with accurate power data being calculated and transmitted as expected. The next steps will involve further fine-tuning and full integration with the other systems.


