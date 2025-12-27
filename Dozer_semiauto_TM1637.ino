#include <TimerMs.h>  //библ таймера
#include <EncButton.h> //библ энкодер-кнопка
#include <GyverTM1637.h> //библ индикатора
//объявл. констант
#define dtmin 100    // dtmax, dtmin - диапазон разрешенных значений времени, все в мсек
#define dtmax 4000   //время макс — для отладки 1 сек, фактически бывает до 4 секунд
#define pauz 200     //принуд. пауза после налива, защита от случ. нажатия
#define valvepin 12  //назначен вывод клапана
#define CLK 6 
#define DIO 7
//настройки encbutton
#define EB_NO_FOR
#define EB_NO_COUNTER


GyverTM1637 disp(CLK, DIO);
//объявл. переменных
int stp = 25;   //stp инкремент в мсек на шаг
int dt = 450; // dt — заданное изначально время сработки таймера
TimerMs doz(dt, 0, 1); //инициализация таймера dt - уст время, 0 остановлен, 1 - режим "таймер" // (период, мсек ), (0 не запущен / 1 запущен), (режим: 0 период / 1 таймер)
EncButton eb(2, 3, 4); // энкодер на 2-3-4 пины
Button btn5(5, INPUT, LOW); // кнопка "старт" на 5 пин, режим внешняя подтяжка Input, Уровень срабатывания LOW

void setup() {
  //Serial.begin(115200);
  doz.setTimerMode(); //режим однократный обратный отсчет
  doz.stop(); // принудтельно стоп таймер
  //пишем значения нач конфигурации в порт для отладки:
  //Serial.println(String("dt: ") + dt + "; dtmin: " + dtmin + "; dtmax: " + dtmax + "; step: " + stp + "; pauz: " + pauz + ";"); 
  disp.clear();
  disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум)
  disp.displayInt(dt);
  pinMode(valvepin, OUTPUT);  //настройка пина клапан на выход
  digitalWrite(valvepin, LOW); //сброс valvepin пина в 0

}

void loop() {
   eb.tick(); //опрос энкодера
   btn5.tick(); //опрос кнопки

      //обработка поворота энкодера
       if (eb.turn()) {
        switch (eb.pressing()) {  // условие нажато / не нажато
        case false: // простой поворот без нажатия - инкремент времени таймера
        time_increment();
         break;
            
         case true: //отработка поворота с нажатием 
    //     Serial.println(String("Поворот с нажатием"));
         break;
      }
   }
    if (btn5.press()) {
      //Serial.println("Button press"); //вывод в отладку
      fill_timer(); //вызов подпрограммы наполнения "полуавтомат"
      }
}

//подпрограмма наполнения "полуавтомат"
void fill_timer() {
    doz.setTime(dt); //передаем в таймер текущее значение периода
    doz.start(); //запуск отсчета
    digitalWrite(valvepin, HIGH); //вкл клапан
    //Serial.println(String("Fill active: ") + doz.timeLeft()); //вывод в порт отладки
    while (doz.status() == true); //крутимся на месте ждем флаг окончания таймера
    digitalWrite(valvepin, LOW); // выкл клапан
    //Serial.println(String("Fill stopped")); //вывод в порт отладки
    doz.setTime(pauz); // пауза pauz мсек от быстрого повторного нажатия кнопки 
    doz.start(); // запуск таймера на pauz мсек
    //Serial.println(String("pauz пуск: ") + doz.timeLeft());
    while (doz.status() == true); //крутимся на месте ждем флаг окончания таймера
    //Serial.println(String("pauz всё")); //вывод в порт отладки
}

// подпрограмма настройки времени энкодером
void time_increment() {
       
        stp = (dt > 975) ? 100 : 25; //шаг инкремента зависит от значения времени сработки
        dt += stp * eb.dir(); //изменяем период на инкремент со знаком направления вращения энкодера
      //  Serial.println(String("dt before: ") + dt); //вывод отладки до расчета
        dt = (dt < dtmin) ? (dtmin) : dt; // условие меньше меньшего
        dt = (dt > dtmax) ? (dtmax) : dt; //условие больше большего
        //Serial.println(String("dt after: ") + dt); //вывод отладки после расчета
        disp.clear();
        disp.displayInt(dt);
    
}
