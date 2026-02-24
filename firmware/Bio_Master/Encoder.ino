void encoder () {
  enc1.tick();
  val = true;
  if (enc1.isTurn()) {
    int increment = 0;  // локальная переменная направления
    
    // получаем направление   
    if (enc1.isRight()) increment = 1;
    if (enc1.isLeft()) increment = -1;
    arrowPos += increment;  // двигаем курсор  
    arrowPos = constrain(arrowPos, 0, SETTINGS_AMOUNT - 1);
    printgui();
  }
      if (enc1.isClick()) {
    switch (arrowPos) {
      case 0: pamidor(); break;
      case 1: sabzi(); break;
      case 2: kartoshka(); break;
      case 3: piyoz(); break;
      case 4: baqlajon(); break;
      case 5: karam(); break;
      case 6: qovoq(); break;
      case 7: tarvuz(); break;
      case 8: qavun(); break;
      case 9: bodring(); break;
      case 10: boshqalar(); break;
      case 11: chiqish (); break;
      // добавьте другие кейсы
    }
    delay(300);  // задержка для предотвращения дребезга кнопки
  }
}
void printgui () {
    lcd.clear();  
    screenPos = arrowPos / LINES;   // ищем номер экрана (0..3 - 0, 4..7 - 1)

    for (byte i = 0; i < LINES; i++) {  // для всех строк
      lcd.setCursor(0, i);              // курсор в начало

      // если курсор находится на выбранной строке
      if (arrowPos == LINES * screenPos + i) lcd.write(126);  // рисуем стрелку
      else lcd.write(32);     // рисуем пробел

      // если пункты меню закончились, покидаем цикл for
      if (LINES * screenPos + i == SETTINGS_AMOUNT) break;

      // выводим имя и значение пункта меню
      lcd.print(settingsNames[LINES * screenPos + i]);
    }
}
