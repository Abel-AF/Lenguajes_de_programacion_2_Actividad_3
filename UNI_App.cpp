#include <windows.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sql.h>
#include <sqlext.h>
#pragma comment(lib, "odbc32.lib")
using namespace std;

class ConexionODBC {
private:
    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt;
    SQLRETURN ret;

public:
    ConexionODBC() {
        henv = SQL_NULL_HENV;
        hdbc = SQL_NULL_HDBC;
        hstmt = SQL_NULL_HSTMT;
        ret = 0;

        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
        SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    }

    bool conectar() {
        SQLCHAR connStr[] = "Driver={SQL Server};Server=.;Database=EmpresaUNI;Trusted_Connection=yes;";
        ret = SQLDriverConnect(hdbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
        return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
    }

    void ejecutarQuery(string tabla) {
        if (hstmt != SQL_NULL_HSTMT) {
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        }
        SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
        string query = "SELECT * FROM " + tabla;
        ret = SQLExecDirect(hstmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
            cout << "Error al ejecutar query en la tabla: " << tabla << endl;
        }
    }

    void mostrarError() {
        SQLCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH];
        SQLGetDiagRec(SQL_HANDLE_DBC, this->hdbc, 1, sqlState, NULL, message, SQL_MAX_MESSAGE_LENGTH, NULL);
        cout << "Error SQL: " << message << endl;
    }

    void mostrarCentros() {
        ejecutarQuery("CENTROS");
        cout << "____________________________________________________________________________________________________________________________________________________" << endl;
        cout << " [ CATALOGO DE CENTROS - EMPRESA UNI ] " << endl;
        cout << "____________________________________________________________________________________________________________________________________________________" << endl;
        cout << " num_centro | nombre_centro         | ciudad                                           " << endl;
        cout << "------------+----------------------+---------------------------------------------------" << endl;

        char num_centro[20], nombre_centro[100], ciudad[50];
        SQLBindCol(hstmt, 1, SQL_C_CHAR, num_centro, 10, NULL);
        SQLBindCol(hstmt, 2, SQL_C_CHAR, nombre_centro, 50, NULL);
        SQLBindCol(hstmt, 3, SQL_C_CHAR, ciudad, 50, NULL);

        while (SQLFetch(hstmt) == SQL_SUCCESS) {
            cout << left
                << setw(12) << num_centro
                << setw(22) << nombre_centro
                << setw(20) << ciudad 
                << endl;
        }
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        hstmt = SQL_NULL_HSTMT;
        cout << "____________________________________________________________________________________________________________________________________________________" << endl << endl;
    }

    void mostrarEmpleados() {
        ejecutarQuery("EMPLEADOS");
        cout << "____________________________________________________________________________________________________________________________________________________" << endl;
        cout << " [ CATALOGO DE EMPLEADOS - EMPRESA UNI ] " << endl;
        cout << "____________________________________________________________________________________________________________________________________________________" << endl;
        cout << " Num.Emp | Nombre | Ap.Pat | Ap.Mat | F.Nacim | RFC | Centro | Puesto | Descripcion | Es Directivo  " << endl;
        cout << "---------+--------+--------+--------+---------+-----+--------+--------+-------------+---------------" << endl;

        int id_empleado;               
        char nombre[50], ap_paterno[50], ap_materno[50], fecha_nac[15], rfc[15], fk_num_centro[10], puesto[50], desc_puesto[200];
        bool es_directivo;              

        SQLBindCol(hstmt, 1, SQL_C_SLONG, &id_empleado, 0, NULL);
        SQLBindCol(hstmt, 2, SQL_C_CHAR, nombre, 50, NULL);
        SQLBindCol(hstmt, 3, SQL_C_CHAR, ap_paterno, 50, NULL);
        SQLBindCol(hstmt, 4, SQL_C_CHAR, ap_materno, 50, NULL);
        SQLBindCol(hstmt, 5, SQL_C_CHAR, fecha_nac, 15, NULL);
        SQLBindCol(hstmt, 6, SQL_C_CHAR, rfc, 15, NULL);
        SQLBindCol(hstmt, 7, SQL_C_CHAR, fk_num_centro, 11, NULL);
        SQLBindCol(hstmt, 8, SQL_C_CHAR, puesto, 50, NULL);
        SQLBindCol(hstmt, 9, SQL_C_CHAR, desc_puesto, 200, NULL);
        SQLBindCol(hstmt, 10, SQL_C_BIT, &es_directivo, 0, NULL);

        while (SQLFetch(hstmt) == SQL_SUCCESS) {
            cout << left
                << setw(7) << id_empleado
                << setw(15) << nombre
                << setw(15) << ap_paterno
                << setw(15) << ap_materno
                << setw(12) << fecha_nac
                << setw(15) << rfc
                << setw(7) <<fk_num_centro
                << setw(15) <<desc_puesto
                << setw(5) << es_directivo
                << endl;
        }
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        hstmt = SQL_NULL_HSTMT;
        cout << "____________________________________________________________________________________________________________________________________________________" << endl << endl;
    }

    void mostrarDirectivos() {
        ejecutarQuery("DIRECTIVOS");
        cout << "____________________________________________________________________________________________________________________________________________________" << endl;
        cout << " [ CATALOGO DE DIRECTIVOS - EMPRESA UNI ] " << endl;
        cout << "____________________________________________________________________________________________________________________________________________________" << endl;
        cout << " id_empleado | num_centro_supervisa | prestacion_comb                                  " << endl;
        cout << "-------------+---------------------+-------------------------------" << endl;


        int id_Emp, num_centro_supervisa;
        bool prestacion_combustible;

        SQLBindCol(hstmt, 1, SQL_C_SLONG, &id_Emp, 0, NULL);
        SQLBindCol(hstmt, 2, SQL_C_SLONG, &num_centro_supervisa, 0, NULL);
        SQLBindCol(hstmt, 3, SQL_C_BIT, &prestacion_combustible, 0, NULL);

        while (SQLFetch(hstmt) == SQL_SUCCESS) {
            cout << left 
                << setw(13) << id_Emp
                << setw(19) << num_centro_supervisa
                << (prestacion_combustible ?"si":"No") << endl;
        }
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        hstmt = SQL_NULL_HSTMT;
        cout << "____________________________________________________________________________________________________________________________________________________" << endl << endl;
    }

    void mostrarPuestos() {
        ejecutarQuery("PUESTOS");
        cout << "____________________________________________________________________________________________________________________________________________________" << endl;
        cout << " [ CATALOGO DE PUESTOS - EMPRESA UNI ] " << endl;
        cout << "____________________________________________________________________________________________________________________________________________________" << endl;
        cout << " numero_puesto | puesto         | descripcion                                              " << endl;
        cout << "---------------+----------------+----------------------------------------------------------" << endl;

        int numero_puesto;
        char puesto[50], descripcion[200];

        SQLBindCol(hstmt, 1, SQL_C_SLONG, &numero_puesto, 0, NULL);
        SQLBindCol(hstmt, 2, SQL_C_CHAR, puesto, 50, NULL);
        SQLBindCol(hstmt, 3, SQL_C_CHAR, descripcion, 200, NULL);

        while (SQLFetch(hstmt) == SQL_SUCCESS) {
            cout << left << setw(15) << numero_puesto
                << setw(16) << puesto
                << descripcion << endl;
        }
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        hstmt = SQL_NULL_HSTMT;
        cout << "____________________________________________________________________________________________________________________________________________________" << endl << endl;
    }

    void desconectar() {
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
        SQLDisconnect(hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
    }
};

int main() {
    setlocale(LC_ALL, "spanish");
    cout << "=================================================================" << endl;
    cout << " SISTEMA DE GESTION - EMPRESA UNI - ACTIVIDAD 3" << endl;
    cout << "=================================================================" << endl << endl;

    ConexionODBC conn;
    if (conn.conectar()) {
        cout << " CONEXION ODBC EXITOSA" << endl << endl;

        conn.mostrarPuestos();
        conn.mostrarCentros();
        conn.mostrarEmpleados();
        conn.mostrarDirectivos();
        conn.desconectar();
    }
    else {
        cout << " ERROR CONEXION ODBC" << endl;
        conn.mostrarError();
    }
}