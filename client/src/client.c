#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger("tp0.log");

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
    log_info(logger, "Hola! Soy un log");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config("cliente.config");
    
	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	valor = config_get_string_value(config, "CLAVE");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// Loggeamos el valor de config
    
    log_info(logger, valor);

	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	uint32_t handshake = 1;
    uint32_t result;

    send(conexion, &handshake, sizeof(uint32_t), 0);
    recv(conexion, &result, sizeof(uint32_t), MSG_WAITALL);

    if(result == -1){
        log_error(logger, "No se pudo realizar el handshake \n");
    }else if (result == 0) {
        log_info(logger, "Handshake realizado con exito \n");
    }

	// Enviamos al servidor el valor de CLAVE como mensaje
    enviar_mensaje(valor, conexion);

	log_info(logger, "Mensaje enviado \n");

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(char *file)
{   
	char* process_name = "CLIENT";
	bool is_active_console = true;
	t_log* nuevo_logger = log_create(file, process_name, is_active_console, LOG_LEVEL_INFO);

	if(nuevo_logger == NULL){
		perror("Error al crear el logger");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(char *file)
{
	t_config* nuevo_config = config_create(file);
    if(nuevo_config == NULL){
		perror("Error al acceder al config");
		exit(1);
	}
	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío

    while(!string_is_empty(leido)){
		log_info(logger, "%s", leido);
		free(leido);
		leido = readline("> ");
	}

	// ¡No te olvides de liberar las lineas antes de regresar!

}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete

	leido = readline("> ");

	while (!string_equals_ignore_case(leido, ""))
	{
		agregar_a_paquete(paquete, leido, strlen(leido)+1);
		leido = readline("> ");
	}

	enviar_paquete(paquete, conexion);


	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	eliminar_paquete(paquete);
	free(leido);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
    log_destroy(logger);
	config_destroy(config);
    close(conexion);
}
