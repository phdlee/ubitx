/** Menus
 *  The Radio menus are accessed by tapping on the function button. 
 *  - The main loop() constantly looks for a button press and calls doMenu() when it detects
 *  a function button press. 
 *  - As the encoder is rotated, at every 10th pulse, the next or the previous menu
 *  item is displayed. Each menu item is controlled by it's own function.
 *  - Eache menu function may be called to display itself
 *  - Each of these menu routines is called with a button parameter. 
 *  - The btn flag denotes if the menu itme was clicked on or not.
 *  - If the menu item is clicked on, then it is selected,
 *  - If the menu item is NOT clicked on, then the menu's prompt is to be displayed
 */



int menuBand(int btn){
  int knob = 0;
  int band;
  unsigned long offset;

 // band = frequency/1000000l;
 // offset = frequency % 1000000l;
    
  if (!btn){
   printLine2("Band Select?");
   return;
  }

  printLine2("Press to confirm");
  //wait for the button menu select button to be lifted)
  while (btnDown()) {
    delay(50);
    Check_Cat(0);  //To prevent disconnections
  }
  
  delay(50);    
  ritDisable();

  while(!btnDown()){

    knob = enc_read();
    if (knob != 0){
      /*
      if (band > 3 && knob < 0)
        band--;
      if (band < 30 && knob > 0)
        band++; 
      if (band > 10)
        isUSB = true;
      else
        isUSB = false;
      setFrequency(((unsigned long)band * 1000000l) + offset); */
      if (knob < 0 && frequency > 3000000l)
        setFrequency(frequency - 200000l);
      if (knob > 0 && frequency < 30000000l)
        setFrequency(frequency + 200000l);
      if (frequency > 10000000l)
        isUSB = true;
      else
        isUSB = false;
      updateDisplay();
    }
    delay(20);
    Check_Cat(0);  //To prevent disconnections
  }

  while(btnDown()) {
    delay(50);
    Check_Cat(0);  //To prevent disconnections
  }
  
  delay(50);
  
  printLine2("");
  updateDisplay();
  menuOn = 0;
}

//0: default, 1:not use, 2:LSB, 3:USB, 4:CW, 5:AM, 6:FM
byte modeToByte(){
  if (isUSB)
    return 3;
  else
    return 2;
}

void byteToMode(byte modeValue){
  if (modeValue == 3)
    isUSB = 1;
  else
    isUSB = 0;
}

void menuVfoToggle(int btn)
{
  if (!btn){
    if (vfoActive == VFO_A)
      printLine2("Select VFO B?   ");
    else
      printLine2("Select VFO A?   ");    
  }
  else {
      if (vfoActive == VFO_B){
        vfoB = frequency;
        EEPROM.put(VFO_B, vfoB);

        //For save MODE to eeprom
        vfoB_mode = modeToByte();
        EEPROM.put(VFO_B_MODE, vfoB_mode);
        
        vfoActive = VFO_A;
        printLine2("Selected VFO A  ");
        frequency = vfoA;
        byteToMode(vfoA_mode);
      }
      else {
        vfoA = frequency;
        EEPROM.put(VFO_A, frequency);

        //For save MODE to eeprom
        vfoA_mode = modeToByte();
        EEPROM.put(VFO_A_MODE, vfoA_mode);
        
        vfoActive = VFO_B;
        printLine2("Selected VFO B  ");      
        frequency = vfoB;
        byteToMode(vfoB_mode);
      }

      ritDisable();

      updateDisplay();
      printLine2("");
      delay_background(500, 0);
      //exit the menu
      menuOn = 0;
  }
}

void menuRitToggle(int btn){
  if (!btn){
    if (ritOn == 1)
      printLine2("RIT:On, Off?   ");
    else
      printLine2("RIT:Off, On?   ");
  }
  else {
      if (ritOn == 0){
        printLine2("RIT is ON");
        //enable RIT so the current frequency is used at transmit
        ritEnable(frequency);
      }
      else{
        printLine2("RIT is OFF");
        ritDisable();
      }
      menuOn = 0;
      delay_background(500, 0);
      printLine2("");
    updateDisplay();
  }
}

void menuSidebandToggle(int btn){
  if (!btn){
    if (isUSB == true)
      printLine2("Select LSB?");
    else
      printLine2("Select USB?");
  }
  else {
      if (isUSB == true){
        isUSB = false;
        printLine2("LSB Selected");
      }
      else {
        isUSB = true;
        printLine2("USB Selected");
      }
    delay_background(500, 0);
    printLine2("");
    
    updateDisplay();
    menuOn = 0;
  }
}

/**
 * The calibration routines are not normally shown in the menu as they are rarely used
 * They can be enabled by choosing this menu option
 */
void menuSetup(int btn){
  if (!btn){
    if (!modeCalibrate)
      printLine2("Setup On?");
    else
      printLine2("Setup Off?");
  }else {
    if (!modeCalibrate){
      modeCalibrate = true;
      printLine2("Setup:On   ");
    }
    else {
      modeCalibrate = false;
      printLine2("Setup:Off   ");      
    }
   delay_background(2000, 0);
   printLine2("");
   menuOn = 0;
  }
}

void menuExit(int btn){

  if (!btn){
      printLine2("Exit Menu?      ");
  }
  else{
      printLine2("Exiting menu");
      delay_background(300, 0);
      printLine2("");
      updateDisplay();
      menuOn = 0;
  }
}

int menuCWSpeed(int btn){
    int knob = 0;
    int wpm;

    wpm = 1200/cwSpeed;
     
    if (!btn){
     strcpy(b, "CW:");
     itoa(wpm,c, 10);
     strcat(b, c);
     strcat(b, "WPM Change?");
     printLine2(b);
     return;
    }

    printLine1("Press PTT to set");
    strcpy(b, "WPM:");
    itoa(wpm,c, 10);
    strcat(b, c);
    printLine2(b);
    delay_background(300, 0);

    while(!btnDown() && digitalRead(PTT) == HIGH){

      knob = enc_read();
      if (knob != 0){
        if (wpm > 3 && knob < 0)
          wpm--;
        if (wpm < 50 && knob > 0)
          wpm++;

        strcpy(b, "WPM:");
        itoa(wpm,c, 10);
        strcat(b, c);
        printLine2(b);
      }
      //abort if this button is down
      if (btnDown())
        //re-enable the clock1 and clock 2
        break;

      Check_Cat(0);  //To prevent disconnections
    }
    
    //save the setting
    if (digitalRead(PTT) == LOW){
      printLine2("CW Speed set!");
      cwSpeed = 1200/wpm;
      EEPROM.put(CW_SPEED, cwSpeed);
      delay_background(2000, 0);
    }
    printLine2("");
    updateDisplay();
    menuOn = 0;
}

int menuSetupCwDelay(int btn){
    int knob = 0;
    int tmpCWDelay = cwDelayTime * 10;
     
    if (!btn){
     strcpy(b, "CW TX->RX Delay");
     printLine2(b);
     return;
    }

    printLine1("Press PTT to set");
    strcpy(b, "DELAY:");
    itoa(tmpCWDelay,c, 10);
    strcat(b, c);
    printLine2(b);
    delay_background(300, 0);

    while(!btnDown() && digitalRead(PTT) == HIGH){
      knob = enc_read();
      if (knob != 0){
        if (tmpCWDelay > 3 && knob < 0)
          tmpCWDelay -= 10;
        if (tmpCWDelay < 2500 && knob > 0)
          tmpCWDelay += 10;

        strcpy(b, "DELAY:");
        itoa(tmpCWDelay,c, 10);
        strcat(b, c);
        printLine2(b);
      }
      //abort if this button is down
      if (btnDown())
        break;

      Check_Cat(0);  //To prevent disconnections
    }
    
    //save the setting
    if (digitalRead(PTT) == LOW){
      printLine2("CW Delay set!");
      cwDelayTime = tmpCWDelay / 10;
      EEPROM.put(CW_DELAY, cwDelayTime);
      delay_background(2000, 0);
    }
    printLine2("");
    updateDisplay();
    menuOn = 0;
}

int menuSetupTXCWInterval(int btn){
    int knob = 0;
    int tmpTXCWInterval = delayBeforeCWStartTime * 2;
     
    if (!btn){
     strcpy(b, "CW Start Delay");
     printLine2(b);
     return;
    }

    printLine1("Press PTT to set");
    strcpy(b, "Start Delay:");
    itoa(tmpTXCWInterval,c, 10);
    strcat(b, c);
    printLine2(b);
    delay_background(300, 0);

    while(!btnDown() && digitalRead(PTT) == HIGH){
      knob = enc_read();
      if (knob != 0){
        if (tmpTXCWInterval > 0 && knob < 0)
          tmpTXCWInterval -= 2;
        if (tmpTXCWInterval < 500 && knob > 0)
          tmpTXCWInterval += 2;

        strcpy(b, "Start Delay:");
        itoa(tmpTXCWInterval,c, 10);
        strcat(b, c);
        printLine2(b);
      }
      //abort if this button is down
      if (btnDown())
        break;

      Check_Cat(0);  //To prevent disconnections
    }
    
    //save the setting
    if (digitalRead(PTT) == LOW){
      printLine2("CW Start set!");
      delayBeforeCWStartTime = tmpTXCWInterval / 2;
      EEPROM.put(CW_START, delayBeforeCWStartTime);
      delay_background(2000, 0);
    }
    printLine2("");
    updateDisplay();
    menuOn = 0;
}


/**
 * Take a deep breath, math(ematics) ahead
 * The 25 mhz oscillator is multiplied by 35 to run the vco at 875 mhz
 * This is divided by a number to generate different frequencies.
 * If we divide it by 875, we will get 1 mhz signal
 * So, if the vco is shifted up by 875 hz, the generated frequency of 1 mhz is shifted by 1 hz (875/875)
 * At 12 Mhz, the carrier will needed to be shifted down by 12 hz for every 875 hz of shift up of the vco
 * 
 */

 //this is used by the si5351 routines in the ubitx_5351 file
extern int32_t calibration;
extern uint32_t si5351bx_vcoa;

int factoryCalibration(int btn){
  int knob = 0;
  int32_t prev_calibration;


  //keep clear of any previous button press
  while (btnDown())
    delay(100);
  delay(100);
   
  if (!btn){
    printLine2("Set Calibration?");
    return 0;
  }

  prev_calibration = calibration;
  calibration = 0;

  isUSB = true;

  //turn off the second local oscillator and the bfo
  si5351_set_calibration(calibration);
  startTx(TX_CW, 1);
  si5351bx_setfreq(2, 10000000l); 
  
  strcpy(b, "#1 10 MHz cal:");
  ltoa(calibration/8750, c, 10);
  strcat(b, c);
  printLine2(b);     

  while (!btnDown())
  {

    if (digitalRead(PTT) == LOW && !keyDown)
      cwKeydown();
    if (digitalRead(PTT)  == HIGH && keyDown)
      cwKeyUp();
      
    knob = enc_read();

    if (knob > 0)
      calibration += 875;
    else if (knob < 0)
      calibration -= 875;
    else 
      continue; //don't update the frequency or the display
      
    si5351_set_calibration(calibration);
    si5351bx_setfreq(2, 10000000l);
    strcpy(b, "#1 10 MHz cal:");
    ltoa(calibration/8750, c, 10);
    strcat(b, c);
    printLine2(b);     
  }

  cwTimeout = 0;
  keyDown = 0;
  stopTx();

  printLine2("Calibration set!");
  EEPROM.put(MASTER_CAL, calibration);
  initOscillators();
  setFrequency(frequency);    
  updateDisplay();

  while(btnDown())
    delay(50);
  delay(100);
}

int menuSetupCalibration(int btn){
  int knob = 0;
  int32_t prev_calibration;
   
  if (!btn){
    printLine2("Set Calibration?");
    return 0;
  }

  printLine1("Set to Zero-beat,");
  printLine2("press PTT to save");
  delay_background(1000, 0);
  
  prev_calibration = calibration;
  calibration = 0;
  si5351_set_calibration(calibration);
  setFrequency(frequency);    
  
  strcpy(b, "cal:");
  ltoa(calibration/8750, c, 10);
  strcat(b, c);
  printLine2(b);     

  while (digitalRead(PTT) == HIGH && !btnDown())
  {
    knob = enc_read();

    if (knob > 0){
      calibration += 8750;
      usbCarrier += 120;
    }
    else if (knob < 0){
      calibration -= 8750;
      usbCarrier -= 120;
    }
    else
      continue; //don't update the frequency or the display

    si5351_set_calibration(calibration);
    si5351bx_setfreq(0, usbCarrier);
    setFrequency(frequency);    

    strcpy(b, "cal:");
    ltoa(calibration/8750, c, 10);
    strcat(b, c);
    printLine2(b);     
  }
  
  //save the setting
  if (digitalRead(PTT) == LOW){
    printLine1("Calibration set!");
    printLine2("Set Carrier now");
    EEPROM.put(MASTER_CAL, calibration);
    delay_background(2000, 0);
  }
  else
    calibration = prev_calibration;

  printLine2("");
  initOscillators();
  //si5351_set_calibration(calibration);
  setFrequency(frequency);    
  updateDisplay();
  menuOn = 0;
}


void printCarrierFreq(unsigned long freq){

  memset(c, 0, sizeof(c));
  memset(b, 0, sizeof(b));

  ultoa(freq, b, DEC);
  
  strncat(c, b, 2);
  strcat(c, ".");
  strncat(c, &b[2], 3);
  strcat(c, ".");
  strncat(c, &b[5], 1);
  printLine2(c);    
}

void menuSetupCarrier(int btn){
  int knob = 0;
  unsigned long prevCarrier;
   
  if (!btn){
      printLine2("Set the BFO");
    return;
  }

  prevCarrier = usbCarrier;
  printLine1("Tune to best Signal");  
  printLine2("PTT to confirm. ");
  delay_background(1000, 0);

  usbCarrier = 11995000l;
  si5351bx_setfreq(0, usbCarrier);
  printCarrierFreq(usbCarrier);

  //disable all clock 1 and clock 2 
  while (digitalRead(PTT) == HIGH && !btnDown())
  {
    knob = enc_read();

    if (knob > 0)
      usbCarrier -= 50;
    else if (knob < 0)
      usbCarrier += 50;
    else
      continue; //don't update the frequency or the display
      
    si5351bx_setfreq(0, usbCarrier);
    printCarrierFreq(usbCarrier);

    Check_Cat(0);  //To prevent disconnections
    delay(100);
  }

  //save the setting
  if (digitalRead(PTT) == LOW){
    printLine2("Carrier set!    ");
    EEPROM.put(USB_CAL, usbCarrier);
    delay_background(1000, 0);
  }
  else 
    usbCarrier = prevCarrier;

  si5351bx_setfreq(0, usbCarrier);          
  setFrequency(frequency);    
  updateDisplay();
  printLine2("");
  menuOn = 0; 
}

void menuSetupCwTone(int btn){
    int knob = 0;
    int prev_sideTone;
     
    if (!btn){
      printLine2("Change CW Tone");
      return;
    }

    prev_sideTone = sideTone;
    printLine1("Tune CW tone");  
    printLine2("PTT to confirm. ");
    delay_background(1000, 0);
    tone(CW_TONE, sideTone);

    //disable all clock 1 and clock 2 
    while (digitalRead(PTT) == HIGH && !btnDown())
    {
      knob = enc_read();

      if (knob > 0 && sideTone < 2000)
        sideTone += 10;
      else if (knob < 0 && sideTone > 100 )
        sideTone -= 10;
      else
        continue; //don't update the frequency or the display
        
      tone(CW_TONE, sideTone);
      itoa(sideTone, b, 10);
      printLine2(b);

      delay(100);
      Check_Cat(0);  //To prevent disconnections
    }
    noTone(CW_TONE);
    //save the setting
    if (digitalRead(PTT) == LOW){
      printLine2("Sidetone set!    ");
      EEPROM.put(CW_SIDETONE, usbCarrier);
      delay_background(2000, 0);
    }
    else
      sideTone = prev_sideTone;
    
    printLine2("");  
    updateDisplay(); 
    menuOn = 0; 
 }

void setDialLock(byte tmpLock, byte fromMode) {
  isDialLock = tmpLock;

  if (fromMode == 2 || fromMode == 3) return;
  
  if (isDialLock == 1)
    printLine2("Dial Lock ON");
  else
    printLine2("Dial Lock OFF");

  updateDisplay();
  delay_background(1000, 0);
  printLine2("");
}

int btnDownTimeCount;

void doMenu(){
  int select=0, i,btnState;
  
  //for DialLock On/Off function
  btnDownTimeCount = 0;
  
  //wait for the button to be raised up
  while(btnDown()){
    delay(50);
    Check_Cat(0);  //To prevent disconnections
    
    //btnDownTimeCount++;
    //check long time Down Button -> 3 Second
    if (btnDownTimeCount++ > (3000 / 50)) {
      setDialLock(isDialLock == 1 ? 0 : 1, 0); //Reverse Dialo lock
      return;
    }
  }
  delay(50);  //debounce

/*
  //check long time Down Button -> 3 Second
  if (btnDownTimeCount > (3000 / 50)) {
    isDialLock = isDialLock == 1 ? 0 : 1; //Reverse Dialo lock
    if (isDialLock == 1)
      printLine2("Dial Lock ON");
    else
      printLine2("Dial Lock OFF");

    delay_background(1000);
    printLine2("");
    return;
  }
*/
  
  menuOn = 2;
  
  while (menuOn){
    i = enc_read();
    btnState = btnDown();

    if (i > 0){
      if (modeCalibrate && select + i < 130)
        select += i;
      if (!modeCalibrate && select + i < 70)
        select += i;
    }
    if (i < 0 && select - i >= 0)
      select += i;      //caught ya, i is already -ve here, so you add it

    if (select < 10)
      menuBand(btnState);
    else if (select < 20)
      menuRitToggle(btnState);
    else if (select < 30)
      menuVfoToggle(btnState);
    else if (select < 40)
      menuSidebandToggle(btnState);
    else if (select < 50)
      menuCWSpeed(btnState);
    else if (select < 60)
      menuSetup(btnState);
    else if (select < 70 && !modeCalibrate)
      menuExit(btnState);
    else if (select < 80 && modeCalibrate)
      menuSetupCalibration(btnState);   //crystal
    else if (select < 90 && modeCalibrate)
      menuSetupCarrier(btnState);       //lsb
    else if (select < 100 && modeCalibrate)
      menuSetupCwTone(btnState);
    else if (select < 110 && modeCalibrate)
      menuSetupCwDelay(btnState);
    else if (select < 120 && modeCalibrate)
      menuSetupTXCWInterval(btnState);
    else if (select < 130 && modeCalibrate)
      menuExit(btnState);

    Check_Cat(0);  //To prevent disconnections
  }

  //debounce the button
  while(btnDown()){
    delay(50);
    Check_Cat(0);  //To prevent disconnections
  }
  delay(50);
}

