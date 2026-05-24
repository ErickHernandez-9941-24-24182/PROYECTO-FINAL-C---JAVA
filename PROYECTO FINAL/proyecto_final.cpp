#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <direct.h>

using namespace std;

struct Proyecto {
    int project_id;
    string title;
    string description;
    int completion_year;
    Proyecto* siguiente;
};

struct Estudiante {
    int student_id;
    string full_name;
    string career;
    int semester;
    float gpa;
    int skill_score;
    Proyecto* proyectos;
};

struct HashNode {
    Estudiante* estudiante;
    HashNode* siguiente;
};

struct AVLNode {
    Estudiante* estudiante;
    AVLNode* izquierda;
    AVLNode* derecha;
    int altura;
};

struct AdjNode {
    int student_id;
    AdjNode* siguiente;
};

struct GraphNode {
    int student_id;
    AdjNode* adyacentes;
};

struct QueueNode {
    int valor;
    QueueNode* siguiente;
};

struct Cola {
    QueueNode* frente;
    QueueNode* final;
};

struct StackNode {
    string accion;
    StackNode* siguiente;
};

const int HASH_SIZE = 101;

HashNode* tablaHash[HASH_SIZE];
AVLNode* raizAVL = nullptr;
vector<GraphNode> grafo;
StackNode* historial = nullptr;

int totalEstudiantes = 0;
int totalConexiones = 0;
int totalProyectos = 0;
int totalColisiones = 0;
double tiempoCarga = 0.0;

void pausar() {
    cout << "\nPresione ENTER para continuar...";
    cin.ignore();
    cin.get();
}

void limpiarEntrada() {
    cin.clear();
    cin.ignore(10000, '\n');
}

void agregarHistorial(string accion) {
    StackNode* nuevo = new StackNode;
    nuevo->accion = accion;
    nuevo->siguiente = historial;
    historial = nuevo;
}

void mostrarEncabezado() {
    cout << "\n===== RED SOCIAL ACADEMICA =====\n";
    cout << "NOMBRE: ERICK RUBEN HERNANDEZ CHAVEZ\n";
    cout << "CARNET: 9941-24-24182\n";
    cout << "CARRERA: ING EN SISTEMAS\n";
    cout << "--------------------------------\n";
}

int funcionHash(int id) {
    return id % HASH_SIZE;
}

Estudiante* crearEstudiante(int id, string nombre, string carrera, int semestre, float gpa, int score) {
    Estudiante* nuevo = new Estudiante;
    nuevo->student_id = id;
    nuevo->full_name = nombre;
    nuevo->career = carrera;
    nuevo->semester = semestre;
    nuevo->gpa = gpa;
    nuevo->skill_score = score;
    nuevo->proyectos = nullptr;
    return nuevo;
}

Estudiante* buscarHash(int id) {
    int indice = funcionHash(id);
    HashNode* actual = tablaHash[indice];

    while (actual != nullptr) {
        if (actual->estudiante->student_id == id) {
            return actual->estudiante;
        }
        actual = actual->siguiente;
    }

    return nullptr;
}

bool insertarHash(Estudiante* estudiante) {
    if (buscarHash(estudiante->student_id) != nullptr) {
        return false;
    }

    int indice = funcionHash(estudiante->student_id);

    if (tablaHash[indice] != nullptr) {
        totalColisiones++;
    }

    HashNode* nuevo = new HashNode;
    nuevo->estudiante = estudiante;
    nuevo->siguiente = tablaHash[indice];
    tablaHash[indice] = nuevo;

    totalEstudiantes++;
    return true;
}

void eliminarProyectos(Proyecto*& cabeza) {
    Proyecto* actual = cabeza;

    while (actual != nullptr) {
        Proyecto* temp = actual;
        actual = actual->siguiente;
        delete temp;
    }

    cabeza = nullptr;
}

bool eliminarHash(int id) {
    int indice = funcionHash(id);
    HashNode* actual = tablaHash[indice];
    HashNode* anterior = nullptr;

    while (actual != nullptr) {
        if (actual->estudiante->student_id == id) {
            if (anterior == nullptr) {
                tablaHash[indice] = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }

            eliminarProyectos(actual->estudiante->proyectos);
            delete actual->estudiante;
            delete actual;
            totalEstudiantes--;
            return true;
        }

        anterior = actual;
        actual = actual->siguiente;
    }

    return false;
}

int altura(AVLNode* nodo) {
    if (nodo == nullptr) {
        return 0;
    }

    return nodo->altura;
}

int maximo(int a, int b) {
    return (a > b) ? a : b;
}

int obtenerBalance(AVLNode* nodo) {
    if (nodo == nullptr) {
        return 0;
    }

    return altura(nodo->izquierda) - altura(nodo->derecha);
}

AVLNode* rotacionDerecha(AVLNode* y) {
    AVLNode* x = y->izquierda;
    AVLNode* t2 = x->derecha;

    x->derecha = y;
    y->izquierda = t2;

    y->altura = maximo(altura(y->izquierda), altura(y->derecha)) + 1;
    x->altura = maximo(altura(x->izquierda), altura(x->derecha)) + 1;

    return x;
}

AVLNode* rotacionIzquierda(AVLNode* x) {
    AVLNode* y = x->derecha;
    AVLNode* t2 = y->izquierda;

    y->izquierda = x;
    x->derecha = t2;

    x->altura = maximo(altura(x->izquierda), altura(x->derecha)) + 1;
    y->altura = maximo(altura(y->izquierda), altura(y->derecha)) + 1;

    return y;
}

AVLNode* crearNodoAVL(Estudiante* estudiante) {
    AVLNode* nuevo = new AVLNode;
    nuevo->estudiante = estudiante;
    nuevo->izquierda = nullptr;
    nuevo->derecha = nullptr;
    nuevo->altura = 1;
    return nuevo;
}

bool menorAVL(Estudiante* a, Estudiante* b) {
    if (a->skill_score < b->skill_score) {
        return true;
    }

    if (a->skill_score == b->skill_score && a->student_id < b->student_id) {
        return true;
    }

    return false;
}

AVLNode* insertarAVL(AVLNode* nodo, Estudiante* estudiante) {
    if (nodo == nullptr) {
        return crearNodoAVL(estudiante);
    }

    if (menorAVL(estudiante, nodo->estudiante)) {
        nodo->izquierda = insertarAVL(nodo->izquierda, estudiante);
    } else {
        nodo->derecha = insertarAVL(nodo->derecha, estudiante);
    }

    nodo->altura = 1 + maximo(altura(nodo->izquierda), altura(nodo->derecha));

    int balance = obtenerBalance(nodo);

    if (balance > 1 && menorAVL(estudiante, nodo->izquierda->estudiante)) {
        return rotacionDerecha(nodo);
    }

    if (balance < -1 && !menorAVL(estudiante, nodo->derecha->estudiante)) {
        return rotacionIzquierda(nodo);
    }

    if (balance > 1 && !menorAVL(estudiante, nodo->izquierda->estudiante)) {
        nodo->izquierda = rotacionIzquierda(nodo->izquierda);
        return rotacionDerecha(nodo);
    }

    if (balance < -1 && menorAVL(estudiante, nodo->derecha->estudiante)) {
        nodo->derecha = rotacionDerecha(nodo->derecha);
        return rotacionIzquierda(nodo);
    }

    return nodo;
}

AVLNode* nodoMinimo(AVLNode* nodo) {
    AVLNode* actual = nodo;

    while (actual->izquierda != nullptr) {
        actual = actual->izquierda;
    }

    return actual;
}

AVLNode* eliminarAVL(AVLNode* raiz, int score, int id) {
    if (raiz == nullptr) {
        return raiz;
    }

    if (score < raiz->estudiante->skill_score ||
        (score == raiz->estudiante->skill_score && id < raiz->estudiante->student_id)) {
        raiz->izquierda = eliminarAVL(raiz->izquierda, score, id);
    } else if (score > raiz->estudiante->skill_score ||
        (score == raiz->estudiante->skill_score && id > raiz->estudiante->student_id)) {
        raiz->derecha = eliminarAVL(raiz->derecha, score, id);
    } else {
        if (raiz->izquierda == nullptr || raiz->derecha == nullptr) {
            AVLNode* temp = raiz->izquierda ? raiz->izquierda : raiz->derecha;

            if (temp == nullptr) {
                temp = raiz;
                raiz = nullptr;
            } else {
                *raiz = *temp;
            }

            delete temp;
        } else {
            AVLNode* temp = nodoMinimo(raiz->derecha);
            raiz->estudiante = temp->estudiante;
            raiz->derecha = eliminarAVL(raiz->derecha, temp->estudiante->skill_score, temp->estudiante->student_id);
        }
    }

    if (raiz == nullptr) {
        return raiz;
    }

    raiz->altura = 1 + maximo(altura(raiz->izquierda), altura(raiz->derecha));

    int balance = obtenerBalance(raiz);

    if (balance > 1 && obtenerBalance(raiz->izquierda) >= 0) {
        return rotacionDerecha(raiz);
    }

    if (balance > 1 && obtenerBalance(raiz->izquierda) < 0) {
        raiz->izquierda = rotacionIzquierda(raiz->izquierda);
        return rotacionDerecha(raiz);
    }

    if (balance < -1 && obtenerBalance(raiz->derecha) <= 0) {
        return rotacionIzquierda(raiz);
    }

    if (balance < -1 && obtenerBalance(raiz->derecha) > 0) {
        raiz->derecha = rotacionDerecha(raiz->derecha);
        return rotacionIzquierda(raiz);
    }

    return raiz;
}

void rankingDescendente(AVLNode* nodo) {
    if (nodo == nullptr) {
        return;
    }

    rankingDescendente(nodo->derecha);

    cout << "ID: " << nodo->estudiante->student_id
         << " | Nombre: " << nodo->estudiante->full_name
         << " | Skill Score: " << nodo->estudiante->skill_score
         << " | GPA: " << fixed << setprecision(2) << nodo->estudiante->gpa << endl;

    rankingDescendente(nodo->izquierda);
}

void inOrdenAVL(AVLNode* nodo) {
    if (nodo == nullptr) {
        return;
    }

    inOrdenAVL(nodo->izquierda);

    cout << "ID: " << nodo->estudiante->student_id
         << " | Score: " << nodo->estudiante->skill_score
         << " | Nombre: " << nodo->estudiante->full_name << endl;

    inOrdenAVL(nodo->derecha);
}

void buscarScoreAVL(AVLNode* nodo, int score, bool& encontrado) {
    if (nodo == nullptr) {
        return;
    }

    buscarScoreAVL(nodo->izquierda, score, encontrado);

    if (nodo->estudiante->skill_score == score) {
        cout << "ID: " << nodo->estudiante->student_id
             << " | Nombre: " << nodo->estudiante->full_name
             << " | Score: " << nodo->estudiante->skill_score << endl;
        encontrado = true;
    }

    buscarScoreAVL(nodo->derecha, score, encontrado);
}

int buscarNodoGrafo(int id) {
    for (int i = 0; i < (int)grafo.size(); i++) {
        if (grafo[i].student_id == id) {
            return i;
        }
    }

    return -1;
}

void agregarNodoGrafo(int id) {
    if (buscarNodoGrafo(id) != -1) {
        return;
    }

    GraphNode nuevo;
    nuevo.student_id = id;
    nuevo.adyacentes = nullptr;
    grafo.push_back(nuevo);
}

bool existeAdyacente(AdjNode* lista, int id) {
    AdjNode* actual = lista;

    while (actual != nullptr) {
        if (actual->student_id == id) {
            return true;
        }

        actual = actual->siguiente;
    }

    return false;
}

void agregarAdyacente(AdjNode*& lista, int id) {
    AdjNode* nuevo = new AdjNode;
    nuevo->student_id = id;
    nuevo->siguiente = lista;
    lista = nuevo;
}

bool conectarEstudiantes(int id1, int id2) {
    if (id1 == id2) {
        cout << "No se puede conectar un estudiante consigo mismo.\n";
        return false;
    }

    if (buscarHash(id1) == nullptr || buscarHash(id2) == nullptr) {
        cout << "Uno o ambos estudiantes no existen.\n";
        return false;
    }

    int pos1 = buscarNodoGrafo(id1);
    int pos2 = buscarNodoGrafo(id2);

    if (pos1 == -1 || pos2 == -1) {
        cout << "Error en grafo.\n";
        return false;
    }

    if (existeAdyacente(grafo[pos1].adyacentes, id2)) {
        cout << "La conexion ya existe.\n";
        return false;
    }

    agregarAdyacente(grafo[pos1].adyacentes, id2);
    agregarAdyacente(grafo[pos2].adyacentes, id1);

    totalConexiones++;
    agregarHistorial("Conexion creada");
    return true;
}

void mostrarConexiones(int id) {
    int pos = buscarNodoGrafo(id);

    if (pos == -1) {
        cout << "Estudiante no encontrado en el grafo.\n";
        return;
    }

    cout << "Conexiones del estudiante " << id << ":\n";

    AdjNode* actual = grafo[pos].adyacentes;

    if (actual == nullptr) {
        cout << "No tiene conexiones.\n";
        return;
    }

    while (actual != nullptr) {
        Estudiante* est = buscarHash(actual->student_id);

        if (est != nullptr) {
            cout << "ID: " << est->student_id << " | Nombre: " << est->full_name << endl;
        }

        actual = actual->siguiente;
    }
}

bool verificarConexion(int id1, int id2) {
    int pos = buscarNodoGrafo(id1);

    if (pos == -1) {
        return false;
    }

    return existeAdyacente(grafo[pos].adyacentes, id2);
}

void eliminarAdyacente(AdjNode*& lista, int id) {
    AdjNode* actual = lista;
    AdjNode* anterior = nullptr;

    while (actual != nullptr) {
        if (actual->student_id == id) {
            if (anterior == nullptr) {
                lista = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }

            delete actual;
            return;
        }

        anterior = actual;
        actual = actual->siguiente;
    }
}

void eliminarEstudianteGrafo(int id) {
    int pos = buscarNodoGrafo(id);

    if (pos == -1) {
        return;
    }

    AdjNode* actual = grafo[pos].adyacentes;

    while (actual != nullptr) {
        int conectado = actual->student_id;
        int posConectado = buscarNodoGrafo(conectado);

        if (posConectado != -1) {
            eliminarAdyacente(grafo[posConectado].adyacentes, id);
            totalConexiones--;
        }

        actual = actual->siguiente;
    }

    actual = grafo[pos].adyacentes;

    while (actual != nullptr) {
        AdjNode* temp = actual;
        actual = actual->siguiente;
        delete temp;
    }

    grafo.erase(grafo.begin() + pos);
}

void inicializarCola(Cola& c) {
    c.frente = nullptr;
    c.final = nullptr;
}

bool colaVacia(Cola& c) {
    return c.frente == nullptr;
}

void encolar(Cola& c, int valor) {
    QueueNode* nuevo = new QueueNode;
    nuevo->valor = valor;
    nuevo->siguiente = nullptr;

    if (colaVacia(c)) {
        c.frente = nuevo;
        c.final = nuevo;
    } else {
        c.final->siguiente = nuevo;
        c.final = nuevo;
    }
}

int desencolar(Cola& c) {
    if (colaVacia(c)) {
        return -1;
    }

    QueueNode* temp = c.frente;
    int valor = temp->valor;
    c.frente = c.frente->siguiente;

    if (c.frente == nullptr) {
        c.final = nullptr;
    }

    delete temp;
    return valor;
}

bool visitado(vector<int>& visitados, int id) {
    for (int i = 0; i < (int)visitados.size(); i++) {
        if (visitados[i] == id) {
            return true;
        }
    }

    return false;
}

void bfs(int inicio) {
    if (buscarNodoGrafo(inicio) == -1) {
        cout << "Estudiante no encontrado.\n";
        return;
    }

    vector<int> visitados;
    Cola cola;
    inicializarCola(cola);

    encolar(cola, inicio);
    visitados.push_back(inicio);

    cout << "Recorrido BFS: ";

    while (!colaVacia(cola)) {
        int actualID = desencolar(cola);
        cout << actualID << " ";

        int pos = buscarNodoGrafo(actualID);
        AdjNode* actual = grafo[pos].adyacentes;

        while (actual != nullptr) {
            if (!visitado(visitados, actual->student_id)) {
                visitados.push_back(actual->student_id);
                encolar(cola, actual->student_id);
            }

            actual = actual->siguiente;
        }
    }

    cout << endl;
}

void dfsRecursivo(int id, vector<int>& visitados) {
    visitados.push_back(id);
    cout << id << " ";

    int pos = buscarNodoGrafo(id);
    AdjNode* actual = grafo[pos].adyacentes;

    while (actual != nullptr) {
        if (!visitado(visitados, actual->student_id)) {
            dfsRecursivo(actual->student_id, visitados);
        }

        actual = actual->siguiente;
    }
}

void dfs(int inicio) {
    if (buscarNodoGrafo(inicio) == -1) {
        cout << "Estudiante no encontrado.\n";
        return;
    }

    vector<int> visitados;

    cout << "Recorrido DFS: ";
    dfsRecursivo(inicio, visitados);
    cout << endl;
}

bool agregarProyecto(int idEstudiante, int project_id, string title, string description, int year) {
    Estudiante* estudiante = buscarHash(idEstudiante);

    if (estudiante == nullptr) {
        cout << "Estudiante no encontrado.\n";
        return false;
    }

    Proyecto* actual = estudiante->proyectos;

    while (actual != nullptr) {
        if (actual->project_id == project_id) {
            cout << "Ya existe un proyecto con ese ID para este estudiante.\n";
            return false;
        }

        actual = actual->siguiente;
    }

    Proyecto* nuevo = new Proyecto;
    nuevo->project_id = project_id;
    nuevo->title = title;
    nuevo->description = description;
    nuevo->completion_year = year;
    nuevo->siguiente = estudiante->proyectos;
    estudiante->proyectos = nuevo;

    totalProyectos++;
    agregarHistorial("Proyecto agregado");
    return true;
}

void mostrarProyectos(int idEstudiante) {
    Estudiante* estudiante = buscarHash(idEstudiante);

    if (estudiante == nullptr) {
        cout << "Estudiante no encontrado.\n";
        return;
    }

    cout << "Proyectos de " << estudiante->full_name << ":\n";

    Proyecto* actual = estudiante->proyectos;

    if (actual == nullptr) {
        cout << "No tiene proyectos registrados.\n";
        return;
    }

    while (actual != nullptr) {
        cout << "ID Proyecto: " << actual->project_id << endl;
        cout << "Titulo: " << actual->title << endl;
        cout << "Descripcion: " << actual->description << endl;
        cout << "Anio: " << actual->completion_year << endl;
        cout << "----------------------\n";

        actual = actual->siguiente;
    }
}

void buscarProyecto(int idEstudiante, int project_id) {
    Estudiante* estudiante = buscarHash(idEstudiante);

    if (estudiante == nullptr) {
        cout << "Estudiante no encontrado.\n";
        return;
    }

    Proyecto* actual = estudiante->proyectos;

    while (actual != nullptr) {
        if (actual->project_id == project_id) {
            cout << "Proyecto encontrado:\n";
            cout << "Titulo: " << actual->title << endl;
            cout << "Descripcion: " << actual->description << endl;
            cout << "Anio: " << actual->completion_year << endl;
            return;
        }

        actual = actual->siguiente;
    }

    cout << "Proyecto no encontrado.\n";
}

void eliminarProyecto(int idEstudiante, int project_id) {
    Estudiante* estudiante = buscarHash(idEstudiante);

    if (estudiante == nullptr) {
        cout << "Estudiante no encontrado.\n";
        return;
    }

    Proyecto* actual = estudiante->proyectos;
    Proyecto* anterior = nullptr;

    while (actual != nullptr) {
        if (actual->project_id == project_id) {
            if (anterior == nullptr) {
                estudiante->proyectos = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }

            delete actual;
            totalProyectos--;
            cout << "Proyecto eliminado correctamente.\n";
            return;
        }

        anterior = actual;
        actual = actual->siguiente;
    }

    cout << "Proyecto no encontrado.\n";
}

void registrarEstudianteManual() {
    int id, semestre, score;
    float gpa;
    string nombre, carrera;

    cout << "ID estudiante: ";
    cin >> id;

    if (buscarHash(id) != nullptr) {
        cout << "No se permiten ID duplicados.\n";
        return;
    }

    limpiarEntrada();

    cout << "Nombre completo: ";
    getline(cin, nombre);

    cout << "Carrera: ";
    getline(cin, carrera);

    cout << "Semestre: ";
    cin >> semestre;

    cout << "GPA: ";
    cin >> gpa;

    cout << "Skill score: ";
    cin >> score;

    Estudiante* nuevo = crearEstudiante(id, nombre, carrera, semestre, gpa, score);

    if (insertarHash(nuevo)) {
        raizAVL = insertarAVL(raizAVL, nuevo);
        agregarNodoGrafo(id);
        agregarHistorial("Estudiante registrado manualmente");
        cout << "Estudiante registrado correctamente.\n";
    }
}

void mostrarEstudiante(Estudiante* est) {
    if (est == nullptr) {
        cout << "Estudiante no encontrado.\n";
        return;
    }

    cout << "ID: " << est->student_id << endl;
    cout << "Nombre: " << est->full_name << endl;
    cout << "Carrera: " << est->career << endl;
    cout << "Semestre: " << est->semester << endl;
    cout << "GPA: " << fixed << setprecision(2) << est->gpa << endl;
    cout << "Skill Score: " << est->skill_score << endl;
}

void cargarCSV() {
    string ruta;

    char carpetaActual[500];
    _getcwd(carpetaActual, 500);

    cout << "\nEl programa esta buscando archivos desde esta carpeta:\n";
    cout << carpetaActual << endl;

    cout << "\nIngrese ruta del archivo CSV: ";
    limpiarEntrada();
    getline(cin, ruta);

    if (ruta.size() > 1 && ruta.front() == '"' && ruta.back() == '"') {
        ruta = ruta.substr(1, ruta.size() - 2);
    }

    ifstream archivo(ruta);

    if (!archivo.is_open()) {
        cout << "\nNo se pudo abrir el archivo.\n";
        cout << "Ruta escrita: " << ruta << endl;
        cout << "\nSOLUCION:\n";
        cout << "1. Copia el archivo estudiantes.csv\n";
        cout << "2. Pegalo exactamente en esta carpeta:\n";
        cout << carpetaActual << endl;
        cout << "3. Vuelve a ejecutar y escribe solo: estudiantes.csv\n";
        return;
    }

    auto inicio = chrono::high_resolution_clock::now();

    string linea;
    getline(archivo, linea);

    int cargados = 0;

    while (getline(archivo, linea)) {
        if (linea.empty()) {
            continue;
        }

        stringstream ss(linea);
        string idStr, nombre, carrera, semestreStr, gpaStr, scoreStr;

        getline(ss, idStr, ',');
        getline(ss, nombre, ',');
        getline(ss, carrera, ',');
        getline(ss, semestreStr, ',');
        getline(ss, gpaStr, ',');
        getline(ss, scoreStr, ',');

        if (idStr.empty() || nombre.empty() || carrera.empty() || semestreStr.empty() || gpaStr.empty() || scoreStr.empty()) {
            continue;
        }

        int id = stoi(idStr);
        int semestre = stoi(semestreStr);
        float gpa = stof(gpaStr);
        int score = stoi(scoreStr);

        if (buscarHash(id) == nullptr) {
            Estudiante* nuevo = crearEstudiante(id, nombre, carrera, semestre, gpa, score);
            insertarHash(nuevo);
            raizAVL = insertarAVL(raizAVL, nuevo);
            agregarNodoGrafo(id);
            cargados++;
        }
    }

    auto fin = chrono::high_resolution_clock::now();
    tiempoCarga = chrono::duration<double, milli>(fin - inicio).count();

    archivo.close();

    agregarHistorial("Carga de CSV");

    cout << "\nCSV cargado correctamente.\n";
    cout << "Estudiantes cargados: " << cargados << endl;
    cout << "Tiempo de carga: " << tiempoCarga << " ms\n";
}

void eliminarEstudianteSistema() {
    int id;
    cout << "Ingrese ID del estudiante a eliminar: ";
    cin >> id;

    Estudiante* est = buscarHash(id);

    if (est == nullptr) {
        cout << "Estudiante no encontrado.\n";
        return;
    }

    int score = est->skill_score;

    raizAVL = eliminarAVL(raizAVL, score, id);
    eliminarEstudianteGrafo(id);

    if (eliminarHash(id)) {
        agregarHistorial("Estudiante eliminado");
        cout << "Estudiante eliminado de Hash, AVL, Grafo y proyectos.\n";
    }
}

double factorCargaHash() {
    return (double)totalEstudiantes / HASH_SIZE;
}

void mostrarEstadisticas() {
    cout << "\n===== ESTADISTICAS =====\n";
    cout << "Total de estudiantes cargados: " << totalEstudiantes << endl;
    cout << "Total de conexiones creadas: " << totalConexiones << endl;
    cout << "Total de proyectos registrados: " << totalProyectos << endl;
    cout << "Total de colisiones hash: " << totalColisiones << endl;
    cout << "Factor de carga hash: " << fixed << setprecision(2) << factorCargaHash() << endl;
    cout << "Altura actual del AVL: " << altura(raizAVL) << endl;
    cout << "Tiempo total de carga del dataset: " << fixed << setprecision(4) << tiempoCarga << " ms\n";
}

struct BenchHashNode {
    int id;
    BenchHashNode* siguiente;
};

const int BENCH_SIZE = 20011;

int benchHash(int id) {
    return id % BENCH_SIZE;
}

void benchmarkCPP() {
    const int N = 10000;

    ofstream salida("cpp_results.csv");

    if (!salida.is_open()) {
        cout << "No se pudo crear cpp_results.csv\n";
        return;
    }

    salida << "language,operation,structure,records,time_ms\n";

    BenchHashNode* benchTable[BENCH_SIZE];

    for (int i = 0; i < BENCH_SIZE; i++) {
        benchTable[i] = nullptr;
    }

    auto inicioInsertHash = chrono::high_resolution_clock::now();

    for (int i = 1; i <= N; i++) {
        int indice = benchHash(i);
        BenchHashNode* nuevo = new BenchHashNode;
        nuevo->id = i;
        nuevo->siguiente = benchTable[indice];
        benchTable[indice] = nuevo;
    }

    auto finInsertHash = chrono::high_resolution_clock::now();
    double tiempoInsertHash = chrono::duration<double, milli>(finInsertHash - inicioInsertHash).count();

    salida << "C++,insert,HashTable," << N << "," << tiempoInsertHash << "\n";

    auto inicioSearchHash = chrono::high_resolution_clock::now();

    for (int i = 1; i <= N; i++) {
        int indice = benchHash(i);
        BenchHashNode* actual = benchTable[indice];

        while (actual != nullptr) {
            if (actual->id == i) {
                break;
            }
            actual = actual->siguiente;
        }
    }

    auto finSearchHash = chrono::high_resolution_clock::now();
    double tiempoSearchHash = chrono::duration<double, milli>(finSearchHash - inicioSearchHash).count();

    salida << "C++,search,HashTable," << N << "," << tiempoSearchHash << "\n";

    AVLNode* benchAVL = nullptr;
    vector<Estudiante*> estudiantesBench;

    auto inicioInsertAVL = chrono::high_resolution_clock::now();

    for (int i = 1; i <= N; i++) {
        Estudiante* e = crearEstudiante(i, "Estudiante", "Carrera", 1, 80.0f, (i * 37) % 100000);
        estudiantesBench.push_back(e);
        benchAVL = insertarAVL(benchAVL, e);
    }

    auto finInsertAVL = chrono::high_resolution_clock::now();
    double tiempoInsertAVL = chrono::duration<double, milli>(finInsertAVL - inicioInsertAVL).count();

    salida << "C++,insert,AVL," << N << "," << tiempoInsertAVL << "\n";

    auto inicioTraversalAVL = chrono::high_resolution_clock::now();

    vector<AVLNode*> pilaManual;
    AVLNode* actual = benchAVL;

    while (actual != nullptr || !pilaManual.empty()) {
        while (actual != nullptr) {
            pilaManual.push_back(actual);
            actual = actual->izquierda;
        }

        actual = pilaManual.back();
        pilaManual.pop_back();
        actual = actual->derecha;
    }

    auto finTraversalAVL = chrono::high_resolution_clock::now();
    double tiempoTraversalAVL = chrono::duration<double, milli>(finTraversalAVL - inicioTraversalAVL).count();

    salida << "C++,traversal,AVL," << N << "," << tiempoTraversalAVL << "\n";

    vector<GraphNode> benchGraph;

    for (int i = 1; i <= N; i++) {
        GraphNode g;
        g.student_id = i;
        g.adyacentes = nullptr;
        benchGraph.push_back(g);
    }

    for (int i = 0; i < N - 1; i++) {
        agregarAdyacente(benchGraph[i].adyacentes, benchGraph[i + 1].student_id);
        agregarAdyacente(benchGraph[i + 1].adyacentes, benchGraph[i].student_id);
    }

    auto inicioBFS = chrono::high_resolution_clock::now();

    vector<int> visitadosBFS;
    Cola cola;
    inicializarCola(cola);
    encolar(cola, 1);
    visitadosBFS.push_back(1);

    while (!colaVacia(cola)) {
        int id = desencolar(cola);
        int pos = id - 1;

        AdjNode* adj = benchGraph[pos].adyacentes;

        while (adj != nullptr) {
            if (!visitado(visitadosBFS, adj->student_id)) {
                visitadosBFS.push_back(adj->student_id);
                encolar(cola, adj->student_id);
            }

            adj = adj->siguiente;
        }
    }

    auto finBFS = chrono::high_resolution_clock::now();
    double tiempoBFS = chrono::duration<double, milli>(finBFS - inicioBFS).count();

    salida << "C++,bfs,Graph," << N << "," << tiempoBFS << "\n";

    auto inicioDFS = chrono::high_resolution_clock::now();

    vector<int> visitadosDFS;
    vector<int> pilaDFS;
    pilaDFS.push_back(1);

    while (!pilaDFS.empty()) {
        int id = pilaDFS.back();
        pilaDFS.pop_back();

        if (!visitado(visitadosDFS, id)) {
            visitadosDFS.push_back(id);

            int pos = id - 1;
            AdjNode* adj = benchGraph[pos].adyacentes;

            while (adj != nullptr) {
                if (!visitado(visitadosDFS, adj->student_id)) {
                    pilaDFS.push_back(adj->student_id);
                }

                adj = adj->siguiente;
            }
        }
    }

    auto finDFS = chrono::high_resolution_clock::now();
    double tiempoDFS = chrono::duration<double, milli>(finDFS - inicioDFS).count();

    salida << "C++,dfs,Graph," << N << "," << tiempoDFS << "\n";

    salida.close();

    cout << "Benchmark C++ generado correctamente: cpp_results.csv\n";
}

void menu() {
    int opcion;

    do {
        mostrarEncabezado();
        cout << "1. Cargar estudiantes desde CSV\n";
        cout << "2. Registrar estudiante manualmente\n";
        cout << "3. Buscar estudiante por ID\n";
        cout << "4. Eliminar estudiante\n";
        cout << "5. Agregar proyecto a estudiante\n";
        cout << "6. Mostrar proyectos de estudiante\n";
        cout << "7. Conectar estudiantes\n";
        cout << "8. Verificar si dos estudiantes estan conectados\n";
        cout << "9. Mostrar conexiones de estudiante\n";
        cout << "10. Recorrer red con BFS\n";
        cout << "11. Recorrer red con DFS\n";
        cout << "12. Mostrar ranking academico\n";
        cout << "13. Mostrar estadisticas de estructuras\n";
        cout << "14. Generar benchmark C++\n";
        cout << "15. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (cin.fail()) {
            limpiarEntrada();
            cout << "Opcion invalida.\n";
            pausar();
            continue;
        }

        switch (opcion) {
        case 1:
            cargarCSV();
            pausar();
            break;

        case 2:
            registrarEstudianteManual();
            pausar();
            break;

        case 3: {
            int id;
            cout << "Ingrese ID a buscar: ";
            cin >> id;
            mostrarEstudiante(buscarHash(id));
            pausar();
            break;
        }

        case 4:
            eliminarEstudianteSistema();
            pausar();
            break;

        case 5: {
            int idEstudiante, project_id, year;
            string title, description;

            cout << "ID del estudiante: ";
            cin >> idEstudiante;

            cout << "ID del proyecto: ";
            cin >> project_id;

            limpiarEntrada();

            cout << "Titulo: ";
            getline(cin, title);

            cout << "Descripcion: ";
            getline(cin, description);

            cout << "Anio de finalizacion: ";
            cin >> year;

            if (agregarProyecto(idEstudiante, project_id, title, description, year)) {
                cout << "Proyecto agregado correctamente.\n";
            }

            pausar();
            break;
        }

        case 6: {
            int id;
            cout << "ID del estudiante: ";
            cin >> id;
            mostrarProyectos(id);
            pausar();
            break;
        }

        case 7: {
            int id1, id2;
            cout << "ID estudiante 1: ";
            cin >> id1;
            cout << "ID estudiante 2: ";
            cin >> id2;

            if (conectarEstudiantes(id1, id2)) {
                cout << "Conexion creada correctamente.\n";
            }

            pausar();
            break;
        }

        case 8: {
            int id1, id2;
            cout << "ID estudiante 1: ";
            cin >> id1;
            cout << "ID estudiante 2: ";
            cin >> id2;

            if (verificarConexion(id1, id2)) {
                cout << "Los estudiantes si estan conectados.\n";
            } else {
                cout << "Los estudiantes no estan conectados.\n";
            }

            pausar();
            break;
        }

        case 9: {
            int id;
            cout << "ID del estudiante: ";
            cin >> id;
            mostrarConexiones(id);
            pausar();
            break;
        }

        case 10: {
            int id;
            cout << "ID inicial para BFS: ";
            cin >> id;
            bfs(id);
            pausar();
            break;
        }

        case 11: {
            int id;
            cout << "ID inicial para DFS: ";
            cin >> id;
            dfs(id);
            pausar();
            break;
        }

        case 12: {
            int subopcion;
            cout << "\n1. Mostrar ranking descendente\n";
            cout << "2. Mostrar recorrido InOrden\n";
            cout << "3. Buscar por skill_score\n";
            cout << "Seleccione: ";
            cin >> subopcion;

            if (subopcion == 1) {
                rankingDescendente(raizAVL);
            } else if (subopcion == 2) {
                inOrdenAVL(raizAVL);
            } else if (subopcion == 3) {
                int score;
                bool encontrado = false;
                cout << "Ingrese skill_score: ";
                cin >> score;
                buscarScoreAVL(raizAVL, score, encontrado);

                if (!encontrado) {
                    cout << "No se encontraron estudiantes con ese score.\n";
                }
            } else {
                cout << "Opcion invalida.\n";
            }

            pausar();
            break;
        }

        case 13:
            mostrarEstadisticas();
            pausar();
            break;

        case 14:
            benchmarkCPP();
            pausar();
            break;

        case 15:
            cout << "Saliendo del sistema...\n";
            break;

        default:
            cout << "Opcion invalida.\n";
            pausar();
            break;
        }

    } while (opcion != 15);
}

int main() {
    for (int i = 0; i < HASH_SIZE; i++) {
        tablaHash[i] = nullptr;
    }

    menu();

    return 0;
}