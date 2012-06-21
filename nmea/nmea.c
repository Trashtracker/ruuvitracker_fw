#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// GPGGA viestin kentät
static char GGAFixTaken[10];
static char GGALatitude[9];
static char GGALatitudeNS[2];
static char GGALongitude[10];
static char GGALongitudeEW[2];
static char GGAFixQuality[2];
static char GGANumberOfSatellites[3];
static char GGAHorizontalDilutionOfPosition[5];
static char GGAAltitude[8];
static char GGAAltitudeUnit[2];
static char GGAHeightOfGeoid[6];
static char GGAHeightOfGeoidUnit[2];
static char GGALastDGPSUpdate[5];
static char GGADGPSStationID[5];

// GPRMC viestin kentät
static char RMCFixTaken[10];
static char RMCStatus[2];							// Status A=active or V=Void.
static char RMCLatitude[9];
static char RMCLatitudeNS[2];
static char RMCLongitude[10];
static char RMCLongitudeEW[2];
static char RMCSpeed[7];							// Speed over the ground in knots
static char RMCTrackangle[7];					// Track angle in degrees True
static char RMCDate[7];
static char RMCMagneticVariation[7];
static char RMCMagneticVariationEW[2];



int blockcpy(char* dest, const char* srce, int len) {
	int stlen = strlen(srce);
	if (stlen<len) strcpy(dest,srce);
	else *dest=0;
	return stlen+1;
}

// NMEA parseri
// Syötteenä merkkijonon osoite ja pituus.
// esim. yhden merkin syöttö: vNMEAParser(&merkki, 1);
void vNMEAParser(const char* Chr, int Len) {
	static unsigned int Index=0;
	static unsigned char Checksum;
	static unsigned char RCDChecksum;
	static int ChecksumCount;
	static char NMEAMessage[100];
	static int Commas;

	while (Len--) {
		if (*Chr=='$') {				// Paketti alkaa '$' merkistä
			Index=1;
			Checksum=0;
			ChecksumCount = 0;
			Commas = 0;
		}
		else if (Index) {				// Paketin vastaanotto meneillään
			if (ChecksumCount) {		// Tarkistussumman vastanotto meneillään
				if (ChecksumCount==1) {
					RCDChecksum = (*Chr&0xc0?(*Chr&0x07)+10:*Chr-'0')<<4;
					ChecksumCount++;
				}
				else {					// Tarkistussumman toinen merkki, senjälkeen voikin purkaa paketin
					RCDChecksum |= (*Chr&0xc0?(*Chr&0x7)+10:*Chr-'0')&0x0f;
					if (RCDChecksum==Checksum) {
						if (!strcmp ( NMEAMessage, "GPGGA")) {	 // GPGGA
							if (Commas==14) {
								char* ind = NMEAMessage;
								ind += strlen(ind)+1;
								// Muteksi päälle?
								ind += blockcpy(GGAFixTaken, ind, sizeof(GGAFixTaken));
								ind += blockcpy(GGALatitude, ind,  sizeof(GGALatitude));
								ind += blockcpy(GGALatitudeNS, ind, sizeof(GGALatitudeNS));
								ind += blockcpy(GGALongitude, ind, sizeof(GGALongitude));
								ind += blockcpy(GGALongitudeEW, ind, sizeof(GGALongitudeEW));
								ind += blockcpy(GGAFixQuality, ind, sizeof(GGAFixQuality));
								ind += blockcpy(GGANumberOfSatellites, ind, sizeof(GGANumberOfSatellites));
								ind += blockcpy(GGAHorizontalDilutionOfPosition, ind, sizeof(GGAHorizontalDilutionOfPosition));
								ind += blockcpy(GGAAltitude, ind, sizeof(GGAAltitude));
								ind += blockcpy(GGAAltitudeUnit, ind, sizeof(GGAAltitudeUnit));
								ind += blockcpy(GGAHeightOfGeoid, ind, sizeof(GGAHeightOfGeoid));
								ind += blockcpy(GGAHeightOfGeoidUnit, ind, sizeof(GGAHeightOfGeoidUnit));
								ind += blockcpy(GGALastDGPSUpdate, ind, sizeof(GGALastDGPSUpdate));
								blockcpy(GGADGPSStationID, ind, sizeof(GGADGPSStationID));
								// Valmis! Mutexi pois ja ilmoitellaan semafoorilla jonnekin tms?
							}
						}
						else if (!strcmp ( NMEAMessage, "GPRMC")) {	// GPRMC
							if (Commas==11) {
								char* ind = NMEAMessage;
								ind += strlen(ind)+1;
								// Muteksi päälle?
								ind += blockcpy(RMCFixTaken, ind, sizeof(RMCFixTaken));
								ind += blockcpy(RMCStatus, ind, sizeof(RMCStatus));
								ind += blockcpy(RMCLatitude, ind, sizeof(RMCLatitude));
								ind += blockcpy(RMCLatitudeNS, ind, sizeof(RMCLatitudeNS));
								ind += blockcpy(RMCLongitude, ind, sizeof(RMCLongitude));
								ind += blockcpy(RMCLongitudeEW, ind, sizeof(RMCLongitudeEW));
								ind += blockcpy(RMCSpeed, ind, sizeof(RMCSpeed));
								ind += blockcpy(RMCTrackangle, ind, sizeof(RMCTrackangle));
								ind += blockcpy(RMCDate, ind, sizeof(RMCDate));
								ind += blockcpy(RMCMagneticVariation, ind, sizeof(RMCMagneticVariation));
								blockcpy(RMCMagneticVariationEW, ind, sizeof(RMCMagneticVariationEW));
								// Valmis!
							}
						}
					}
					Index=0;
				}
			}
			else if (*Chr=='*') {	 	// Paketti loppuu, vastaanotetaan vielä tarkistussumma.
				NMEAMessage[Index-1]=0;
				ChecksumCount++;
			}
			else {
				if (Index<sizeof(NMEAMessage)) {
					if (*Chr==',') {
						NMEAMessage[Index++-1]=0;
						Commas++;
					}
					else NMEAMessage[Index++-1]=*Chr;
					Checksum ^= *Chr;
				}
				else Index=0;		// Liian pitkä, ei mahdu taulukkoon
			}
		}
		Chr++;
	}
}
