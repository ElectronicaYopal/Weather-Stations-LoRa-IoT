## 1. Crear base de datos *(Usando postgres)*

Construcción de base de datos relacional para almacernar los datos provenientes de los gateway lora.



## 2. Crear API CRUD

Esta recibe las peticiones del gateway lora y almacena los datos en la base de datos.

Para crear una api crud, remítase al siguiente repo: [Ejemplo API CRUD](https://github.com/ElectronicaYopal/Software-y-Ejemplos/blob/main/Ejemplos.md)

```bash
cd go/src/
bee api LoraGps -driver=postgres -conn="postgres://postgres:Electronica2021@127.0.0.1/LoraGps?sslmode=disable"
```
