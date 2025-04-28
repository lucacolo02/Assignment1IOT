// Use this file to store all of the private credentials and connection details

// WiFi configuration
#define SECRET_SSID "IoTLabThingsU14"                  // SSID
#define SECRET_PASS "L@b%I0T*Ui4!P@sS**0%Lessons!"              // WiFi password

// ONLY if static configuration is needed
/*
#define IP {192, 168, 1, 100}                    // IP address
#define SUBNET {255, 255, 255, 0}                // Subnet mask
#define DNS {149, 132, 2, 3}                     // DNS
#define GATEWAY {149, 132, 182, 1}               // Gateway
*/

// MySQL access
#define MYSQL_IP {149, 132, 176, 75}            // IP address of the machine running MySQL
#define MYSQL_USER "MatteoComi"                  // db user
#define MYSQL_PASS "iot886035"              // db user's password

// InfluxDB cfg
#define INFLUXDB_URL "http://149.132.176.75:8086/"   // IP and port of the InfluxDB server
#define INFLUXDB_TOKEN "jcD5WXc6SW22A4_E6gihv3g81tGSLFSzCeBnRHSAEFpuO2475Mo1k58GOhMPY4pBo9PHy2z07t8AINn96SYUrw=="    // API authentication token. Use an existing token or create a new one:
                                                     //  InfluxDB UI -> Load Data -> API Tokens -> Generate API Token -> Custom API Token
                                                     
                                                     //  set a name for the token and select read and write permissions for your buket only!
#define INFLUXDB_ORG "labiot-org"                    // organization id (Use: InfluxDB UI -> Profile -> About -> <name under organization profile> )
#define INFLUXDB_BUCKET "MatteoComi-bucket"       // bucket name (Use: InfluxDB UI -> Load Data -> Buckets)

