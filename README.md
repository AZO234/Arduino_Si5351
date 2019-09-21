Library of clock generator Si5351(I2C bus) by AZO
=================================================
version: v1.0.0(Sep 21, 2019)  
http://domisan.sakura.ne.jp/

Library of Si5351(I2C bus).

First constrast instance of class Si5351_I2C with Wire instance.  
Next setting PLL with setMSx() and PLLSoftReset().  
And setting MS/CLK with setMS().   
Then output with setOutputEnable().

Reference
---------
- Si5351A-B-GT LVCMOS 3 出力クロック・ジェネレータ - Silicon Labs  
https://jp.silabs.com/products/timing/clocks/cmos-clock-generators/device.si5351a-b-gt

- Si5351-B.pdf  
https://www.silabs.com/documents/public/data-sheets/Si5351-B.pdf

- AN619.pdf  
https://www.silabs.com/documents/public/application-notes/AN619.pdf

