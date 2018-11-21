#ifndef protocolostpte_practicas_headerfile
#define protocolostpte_practicas_headerfile
#endif

//COMANDOS DE APLICACI�N

#define HELO "HELO"  //Bienvenida
#define QUIT "QUIT"  //Finalizar conexi�n

#define MAIL "MAIL"   
#define FROM "FROM"  
#define RCPT "RCPT"  
#define TO  "TO:"  
#define SP " "
#define SD "SD"
#define ECHO "ECHO"

// RESPUESTAS A COMANDOS DE APLICACION
#define OK  "OK"
#define ER  "ER"

//FIN DE RESPUESTA
#define CRLF "\r\n"

//ESTADOS
#define S_HELO 0
#define S_MAIL_FROM 1
#define S_RCPT 2
#define S_DATA 3
#define S_MENSAJE 4
#define S_QUIT 5
//HAY QUE A�ADIR ASUNTO
//PUERTO DEL SERVICIO
#define TCP_SERVICE_PORT	25 

// NOMBRE Y PASSWORD AUTORIZADOS
#define USER		"alumno"
#define PASSWORD	"123456"