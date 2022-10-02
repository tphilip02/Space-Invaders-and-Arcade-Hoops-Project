# Final Project Report - Tristan Philip A16400190
***
# Project Objectives 
>The objectives of this project were to improve and build upon a rudimentary controller for the video game 
> *Space Invaders*, and develop a unique design to create a functioning system of our choosing. The video game 
> controller utilized UDP socket messaging to communicate, and had features like vibration upon being hit, an LED
> light indication upon being hit, an OLED displaying the lives and score, displaying high scores before and after each
> game, improved tilt control, decoupled shooting and firing, and a push button to fire bullets. The design challenge I 
> developed was a basketball game that used an IR photosensor to detect made baskets, and keep score within one minute
> of play. The game also displayed high scores before and after the game started, and would display the seconds left in
> the game, the score, and would buzz a motor and light up LEDs each time a point was scored. 

# [*Space Invaders* Controller](https://youtu.be/5uFUN6Wd7k0)
> **Instructions**
> 
>Using my controller is quite straightforward. The controller is held in a landscape orientation, with a push button on 
> the right side. You tilt left to make the ship move left, and you tilt right to make the ship move right. The controls 
> are not always completely fluid (but for the most part are!), so it can take slightly varying angles to get the ship 
> to move in either direction. Use the push button to fire.

>
> 
> **Improvements** 
>
>The first improvement I made to the controller and game was using a push button to shoot instead of tilting the 
> controller downwards. The extra movement of tilting downwards does not let you shoot fast enough, so using a push
> button is quite ergonomic and effective. This improvement was relatively straightforward, and required some edge
> detection logic, as well as proper resetting of a "fired" state (meaning a state of when the button was pressed down, 
> indicating a fired bullet). The second improvement I made to the controller was smoothing the direction changes. There
> were many avenues to make tilt controls more effective, but I chose to leverage online resources and I found a [formula 
> online](https://howtomechatronics.com/tutorials/arduino/how-to-track-orientation-with-arduino-and-adxl345-accelerometer/) 
> (or actually 
> [two](https://www.hobbytronics.co.uk/accelerometer-info#:~:text=If%20you%20want%20to%20measure,one%20axis%20of%20the%20accelerometer.&text=The%20component%20of%20gravity%20acting,y%20axis%20is%20a%20cosine.))
> that calculated angles of pitch and roll from a three-axis accelerometer. I simply utilized the formula within 
> my Arduino code, and utilized a low-pass filter of sorts to stabilize the changes in angle even further, so that when
> the controller was tilted in one direction, it would compare the angles of tilt and determine the orientation from 
> there. This was effective, and quite simple to implement. The third improvement I made was to decouple firing and
> movement by simply sending fire and move commands at the same time, and simply decoupling firing and movement with 
> string parsing using splitting. Then from there, I could move and fire at the same time. I initially had an issue with 
> the buffer being filled with too many messages, but eventually I figured out the issue and properly sent both fire and
> move commands without overloading the buffer. My logic was to essentially only concatenate if both fire and movement 
> was happening simultaneously, and to split only if there was a delimiter present.  
> 
>

>**Features**
> 
> The first feature I implemented was displaying the lives and score of the game on the OLED display. I did so by 
> storing the address from the messages received from the client aka the controller. Then, if there was an address
> received, I send the number of lives and the score to the controller.py code, which I then send to the MCU. The second
> feature I implemented was vibration and the lighting of an LED whenever I get hit by an enemy. This logic was slightly
> more complex. Essentially, in my Arduino code, I parse through the message received from Python for the substring 
> "Lives:", and then I look for the next character. If the number of lives is less than that of the life counter, then 
> I have lost a life. Subsequently, I decrement the lives and the life counter. The LED and motor are on for two seconds 
> and then turn off. The third feature I implemented was the top three [high scores](https://youtu.be/UKXGfXmEAIk) 
> being displayed before and after the 
> game starts (the video shows my girlfriend setting the high score). 
> Essentially, I created a CSV file that I write all the scores to. Then each time I run the program, it
> checks the CSV file for the data, and I find the top three scores by sorting and using slicing to take the last three
> elements. Then I simply print the high scores on the display. This was actually a very interesting problem, and it was
> really cool seeing the code work. It seemed really cool working with a CSV in that manner. 
# [Design Challenge](https://youtu.be/puG41xv_uoY)
> 1. The use case I am trying to address is an alternative to the game *Arcade Hoops*, which is essentially a game where 
> you shoot as many basketballs as you can in one minute, trying to score as many points as possible within the 
> timeframe. I recreated my own rendition of the game, with a one-minute timer that is displayed on the OLED, the points 
> displayed on the OLED, and LEDs and a buzzer motor that are powered on each point scored, as well as on the game ending.
> 
> 
> 2. This problem is worth addressing because no college students can fit a full-sized *Arcade Hoops* game inside a dorm 
> or apartment. However, a small device that you can strap to the back of the rim of a mini hoop would be infinitely more 
> feasible and useful. I personally do have a mini hoop in my dorm, and got the inspiration for this device from playing
> mini basketball with my friends. 
> 
>
> 3. The intended users are probably college students considering the number of college students that have mini hoops in 
> their dorm room. This device would simply be an addition to the mini hoop. However, anybody with a mini hoop could use 
> this device and find utility from it. I would say the primary benefit from this is being able to add it to existing
> mini hoops, and also the space-saving capabilities of it. 
> 
> 
> 4. My solution addresses the needs of the user by being small and compact, yet also completely functional. It can easily
> be added to existing mini hoops, and serves as a cheap, fun alternative to a regular game of *Arcade Hoops* which can 
> be expensive and space-consuming. 


> My thought process was simple: develop an efficient game that could be utilized by anyone. The first part of my program
> opens a CSV filled with the game scores, and sorts and slices the top three [high scores](https://youtu.be/YnqvG-mpIDo), 
> which can then be displayed on 
> the OLED display before the game actually starts. Then, once the user is ready, they can trigger the game to start by
> hitting "enter" on their keyboard to begin the game. From there, I simply receive PPG data from Python in an infinite 
> loop until sixty seconds have elapsed, breaking the loop and entering the "finally" portion of my code. But before I
> get to that, I'll continue to elaborate on my primary code. After receiving the PPG data, I simply need to compare the 
> PPG value to a threshold that I determined from experimental testing (by simply playing the game and determining what 
> threshold would result in consistency with point tracking). I found no need to do something like filter a PPG signal
> because once the PPG values dip below a certain value, a basketball is clearly blocking the sensor and a point has been
> scored. Ideally, I would have two sensors, one above and below the rim, to make sure a basket has been scored. Every time
> a point is scored, I send a message to the MCU saying a point has been scored. Additionally, I send a message with the
> amount of elapsed seconds whenever a second has passed, in addition to the current point totals. Finally, if the time is 
> greater than sixty seconds, I break out of the loop, and print the high scores to the OLED, and add the current score to 
> the CSV file. 
> 
>Within my Arduino code, I check for specific substrings within the message received from Python. If the flags
> "POINT" or "GAME OVER" are found, then I buzz the motor and light up the blue and yellow LEDs in the name of Steph Curry
> and the Golden State Warriors because when you make the ball you're literally Steph Curry for that second. The motor and 
> LED buzz for two seconds and then turn off. The "GAME OVER" flag lights up a red LED and buzzes the motor and turns on the 
> LED for four seconds. If neither of those flags are found and the message received isn't empty, I display the message 
> received on the OLED (which will display the score and seconds elapsed). Some of the biggest issues I had with this 
> system were hardware-related, because serial communication would not work properly 100% of the time, and I sometimes
> would have my MCU reset on the game starting. I eventually realized that was because of a code issue, however. Another 
> issue I had was forgetting to set my pin modes of the motor and LEDs to outputs. Other than that, my system was not 
> overly complicated. 
> 
>
