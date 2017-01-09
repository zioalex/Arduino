void rtcnow () {    
    now = rtc.now();
    #if ECHO_TO_SERIAL    
      Serial.print(now.year(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.day(), DEC);
      Serial.print(" ( ");
      Serial.print(now.dayOfTheWeek(), DEC);
      Serial.print("  ");
      Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
      Serial.print(") ");
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();
      
      Serial.print(" since midnight 1/1/1970 = ");
      Serial.print(now.unixtime());
      Serial.print("s = ");
      Serial.print(now.unixtime() / 86400L);
      Serial.println("d");
    #endif
    
    // calculate a date which is 7 days and 30 seconds into the future
    // DateTime future (now + TimeSpan(7,12,30,6));
    
    // #if ECHO_TO_SERIAL
    //   Serial.print(" now + 7d + 30s: ");
    //   Serial.print(future.year(), DEC);
    //   Serial.print('/');
    //   Serial.print(future.month(), DEC);
    //   Serial.print('/');
    //   Serial.print(future.day(), DEC);
    //   Serial.print(' ');
    //   Serial.print(future.hour(), DEC);
    //   Serial.print(':');
    //   Serial.print(future.minute(), DEC);
    //   Serial.print(':');
    //   Serial.print(future.second(), DEC);
    //   Serial.println();
    //   
    //   Serial.println();
    // #endif
    delay(1000);
}
