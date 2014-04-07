extern void initGyr(void);
extern void setGyrSensitivity(uint16_t);
extern void calculateGyroOffsets(void);
extern void calculateDrift(void);
extern int16_t getGyrValues(void);
extern float getTempValue(void);
extern void calculateAngle(int16_t, uint32_t);
extern void Delay(volatile uint32_t nCount);
