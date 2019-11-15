//printDriverError decodes the CCS811Core::status type and prints the
//type of error to the serial terminal.

//Save the return value of any function of type CCS811Core::status, then pass
//to this function to see what the output was.
void printCCS811DriverError( CCS811Core::status errorCode )
{
  switch ( errorCode )
  {
    case CCS811Core::SENSOR_SUCCESS:
      M5.Lcd.println("SUCCESS");
      break;
    case CCS811Core::SENSOR_ID_ERROR:
      M5.Lcd.println("ID_ERROR");
      break;
    case CCS811Core::SENSOR_I2C_ERROR:
      M5.Lcd.println("I2C_ERROR");
      break;
    case CCS811Core::SENSOR_INTERNAL_ERROR:
      M5.Lcd.println("INTERNAL_ERROR");
      break;
    case CCS811Core::SENSOR_GENERIC_ERROR:
      M5.Lcd.println("GENERIC_ERROR");
      break;
    default:
      M5.Lcd.println("Unspecified error.");
  }
}
