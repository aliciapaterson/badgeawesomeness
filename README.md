# badgeawesomeness
An Apigee-Powered Interactive IoT Badge Thingy

This project implements a Adafruit powered LED badge that can be interacted with via a web page to change LED display patterns and colors.

The BOM includes:
- An Adafruit Feather Huzzah ESP8266- https://www.adafruit.com/product/2821?gclid=CjwKCAiAlfnUBRBQEiwAWpPA6X-3xXjjA4dJr4egL1ABsNV90K4FntXpkiRQP7asOg1HmTIvlF4mexoCy5YQAvD_BwE
- Neopixels (I use a 7-pixel Jewel, a 12 pixel ring and a 24 pixel ring - see https://www.adafruit.com/category/168
- A battery - 3.7v LiPo is enough to power the pixels

The solution stack includes:
 - The Feather Huzzah - see Arduino C code for the program running there
 - Adafruit.io to communicate with the Feather Huzzah (http://adafuit.io)
 - Apigee Edge (http://apigee.com)
 - A PHP front end - see PHP, HTML and CSS code for these items
 
 Although it is possible to use the Feather Huzzah as the backend for Apigee directly, since the badge will be used in environments where the WiF router cannot be managed to create a route or to give the badge a public IP address, using adafruit.io as the backend allows the badge to get updates from adafruit.io and Apigee Edge push events to adafruit.io.
