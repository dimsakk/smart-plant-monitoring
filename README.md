# smart-plant-monitoring
The project consists of two main parts: the SoC, which is an ESP8266, and a website (https://zafora.ece.uowm.gr/~ece01724/) for remote control. The ESP, along with the sensors and a relay, is placed in a basket that can be hung on the edge of a flowerpot using hooks.
The ESP measures soil moisture as well as air temperature and humidity at regular intervals and updates the website. Through the website, the user sets the minimum soil moisture threshold. The ESP receives this value, and when the soil moisture drops below this limit, the ESP turns on a relay, which can be connected to an irrigation system to water the flowerpot.

# Code analysis
Initially, in the ESP8266 code, inside a void setup() function, we turn off the relay as the system boots. Initializations take place, and an attempt is made to connect to WiFi. Then, the Arduino fetches a threshold value from the website, which is the minimum moisture level required to activate the relay.

Next, the void loop() function executes, which starts by calling the void moistureSampling() function and then the void airSampling() function. These two functions use a for loop to take 10 readings of soil moisture and air humidity, and then calculate an average value.

Afterwards, the void CheckMoisture() function is called. This function first compares the soil moisture value obtained by void moistureSampling() with the minimum moisture threshold fetched from the website during the void setup() execution. If the soil moisture value is lower than the threshold, the relay is activated, watering the flowerpot for 5 seconds, followed by a 5-second delay. Otherwise, the relay remains off, and there is simply a 10-second delay.


Following this function, there is an if check that runs every minute, calling the void gethttp() and void Posthttp() functions. In the void gethttp() function, the http.Get() command retrieves the contents of the data.txt file located at "https://zafora.ece.uowm.gr/~ece01724/data.txt". This file is updated with the desired minimum moisture threshold set by the user on the website. Then, checks are performed on the retrieved value to ensure it is valid. In the void Posthttp() function, using http.Get(), we send the temperature, air humidity, and soil moisture values to the website at:
"https://zafora.ece.uowm.gr/~ece01724/update.php?temp=10&hum=20&soil=30".

In the update.php file, we first retrieve data from a form using $_GET and check if the submitted value is null or an int/float using the isset command. Then, the values are stored in an array and saved to the data.json file.

In the index.php file, within the webpage's body, three values are displayed: Air humidity, Air temperature and Soil moisture percentage (at that moment). Below, there is a form where the user can input a new desired value for the minimum soil moisture threshold. Using the readThresholdValue() function, the data.txt file is opened, the user's input value is displayed in the bar, and after the user updates it with the new value, it is saved back to data.txt. Finally, the fetchData() function is called every 2 seconds, which retrieves the latest air humidity, air temperature, and soil moisture data from the data.json file and refreshes the page accordingly.
