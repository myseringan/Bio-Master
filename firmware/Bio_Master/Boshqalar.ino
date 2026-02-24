void boshqalar() {
  val5 = true;
  if (val6 == false) {
    printFUI();
    val6 = true;
  }
  enc1.tick();
  if (enc1.isTurn()) {
    int increment = 0;  // локальная переменная направления
    
    // получаем направление   
    if (enc1.isRight()) increment = 1;
    if (enc1.isLeft()) increment = -1;
    arrowPos1 += increment;  // двигаем курсор  
    arrowPos1 = constrain(arrowPos1, 0, SETTINGS_AMOUNT1 - 1); // ограничиваем
    increment = 0;  // обнуляем инкремент
     if (enc1.isRightH()) increment = 1;
     if (enc1.isLeftH()) increment = -1;
    vals1[arrowPos1] += increment;  // меняем параметры
    printFUI();
  }
  if (arrowPos1 == 5 && enc1.isClick()) {
    T1 = vals1[0]; 
    T2 = vals1[1]; 
    H1 = vals1[2]; 
    H2 = vals1[3]; 
    S =  vals1[4];
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print ("Qabul qilindi!");
        tone(buzzer, 800, 500);
        delay(300);
        noTone(buzzer);
        delay(1000); 
        val5 = false;
        val1 = true;
        val6 = false;
        val = false;
      }
      else if (arrowPos1 == 6 && enc1.isClick()) {
        val5 = false;
        val6 = false;
        val = false;
      }
}

void printFUI() {
  lcd.clear();  
  screenPos1 = arrowPos1 / LINES;   // ищем номер экрана (0..3 - 0, 4..7 - 1)

  for (byte i = 0; i < LINES; i++) {  // для всех строк
    lcd.setCursor(0, i);              // курсор в начало

    // если курсор находится на выбранной строке
    if (arrowPos1 == LINES * screenPos1 + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел

    // если пункты меню закончились, покидаем цикл for
    if (LINES * screenPos1 + i == SETTINGS_AMOUNT1) break;

    // Не выводим ": " и значение для определенных пунктов меню
    if (LINES * screenPos1 + i != 5 && LINES * screenPos1 + i != 6) {
      // выводим имя и значение пункта меню
      lcd.print(settingsNames1[LINES * screenPos1 + i]);
      lcd.print(": ");
      lcd.print(vals1[LINES * screenPos1 + i]);
    }
    else {
      // выводим только имя пункта меню
      lcd.print(settingsNames1[LINES * screenPos1 + i]);
    }
  }
}
