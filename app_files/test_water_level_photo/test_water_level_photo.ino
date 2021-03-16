/***************************************************
* Liquid Level Sensor-FS-IR02
* ****************************************************
* This example is to get liquid level

* @author jackli(Jack.li@dfrobot.com)
* @version  V1.0
* @date  2016-1-30

* GNU Lesser General Public License.
* See <http://www.gnu.org/licenses/> for details.
* All above must be included in any redistribution
* ****************************************************/
int Liquid_level=0;
bool status = false;
void setup() {
 Serial.begin(9600);
 pinMode(5,INPUT);
 pinMode(6, OUTPUT);
}

void loop() {
Liquid_level=digitalRead(5);
Serial.print("Liquid_level= ");Serial.println(Liquid_level,DEC);
digitalWrite(6, status==true?LOW:HIGH);
status=!status;
delay(500);
}
