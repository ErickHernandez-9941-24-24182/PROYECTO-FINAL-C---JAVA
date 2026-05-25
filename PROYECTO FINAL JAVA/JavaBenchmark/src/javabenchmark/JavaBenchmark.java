package javabenchmark;

import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;

public class JavaBenchmark {

    static class Estudiante {
        int studentId;
        String fullName;
        String career;
        int semester;
        double gpa;
        int skillScore;

        public Estudiante(int studentId, String fullName, String career, int semester, double gpa, int skillScore) {
            this.studentId = studentId;
            this.fullName = fullName;
            this.career = career;
            this.semester = semester;
            this.gpa = gpa;
            this.skillScore = skillScore;
        }
    }

    static final int RECORDS = 10000;

    public static void main(String[] args) {
        mostrarEncabezado();
        generarBenchmarkJava();
    }

    public static void mostrarEncabezado() {
        System.out.println("===== BENCHMARK JAVA - RED SOCIAL ACADEMICA =====");
        System.out.println("NOMBRE: ERICK RUBEN HERNANDEZ CHAVEZ");
        System.out.println("CARNET: 9941-24-24182");
        System.out.println("CARRERA: ING EN SISTEMAS");
        System.out.println("------------------------------------------------");
    }

    public static Estudiante crearEstudiante(int id) {
        String nombre = "Estudiante " + id;
        String carrera = "Ingenieria en Sistemas";
        int semestre = (id % 10) + 1;
        double gpa = 60.0 + (id % 41);
        int skillScore = (id * 37) % 100000;

        return new Estudiante(id, nombre, carrera, semestre, gpa, skillScore);
    }

    public static void generarBenchmarkJava() {
        try {
            FileWriter writer = new FileWriter("java_results.csv");

            writer.write("language,operation,structure,records,time_ms\n");

            benchmarkHashMap(writer);
            benchmarkTreeMap(writer);

            writer.close();

            System.out.println();
            System.out.println("Benchmark Java generado correctamente.");
            System.out.println("Archivo creado: java_results.csv");

        } catch (IOException e) {
            System.out.println("Error al generar java_results.csv");
            System.out.println(e.getMessage());
        }
    }

    public static void benchmarkHashMap(FileWriter writer) throws IOException {
        HashMap<Integer, Estudiante> estudiantesHash = new HashMap<>();

        long inicioInsert = System.nanoTime();

        for (int i = 1; i <= RECORDS; i++) {
            Estudiante estudiante = crearEstudiante(i);
            estudiantesHash.put(estudiante.studentId, estudiante);
        }

        long finInsert = System.nanoTime();

        double tiempoInsert = (finInsert - inicioInsert) / 1_000_000.0;

        writer.write("Java,insert,HashMap," + RECORDS + "," + tiempoInsert + "\n");

        long inicioSearch = System.nanoTime();

        for (int i = 1; i <= RECORDS; i++) {
            estudiantesHash.get(i);
        }

        long finSearch = System.nanoTime();

        double tiempoSearch = (finSearch - inicioSearch) / 1_000_000.0;

        writer.write("Java,search,HashMap," + RECORDS + "," + tiempoSearch + "\n");

        System.out.println("HashMap:");
        System.out.println("Insert: " + tiempoInsert + " ms");
        System.out.println("Search: " + tiempoSearch + " ms");
    }

    public static void benchmarkTreeMap(FileWriter writer) throws IOException {
        TreeMap<Integer, ArrayList<Estudiante>> estudiantesTree = new TreeMap<>();

        long inicioInsert = System.nanoTime();

        for (int i = 1; i <= RECORDS; i++) {
            Estudiante estudiante = crearEstudiante(i);
            int score = estudiante.skillScore;

            if (!estudiantesTree.containsKey(score)) {
                estudiantesTree.put(score, new ArrayList<>());
            }

            estudiantesTree.get(score).add(estudiante);
        }

        long finInsert = System.nanoTime();

        double tiempoInsert = (finInsert - inicioInsert) / 1_000_000.0;

        writer.write("Java,insert,TreeMap," + RECORDS + "," + tiempoInsert + "\n");

        long inicioTraversal = System.nanoTime();

        for (Map.Entry<Integer, ArrayList<Estudiante>> entrada : estudiantesTree.entrySet()) {
            ArrayList<Estudiante> lista = entrada.getValue();

            for (Estudiante estudiante : lista) {
                int id = estudiante.studentId;
                int score = estudiante.skillScore;
            }
        }

        long finTraversal = System.nanoTime();

        double tiempoTraversal = (finTraversal - inicioTraversal) / 1_000_000.0;

        writer.write("Java,traversal,TreeMap," + RECORDS + "," + tiempoTraversal + "\n");

        System.out.println();
        System.out.println("TreeMap:");
        System.out.println("Insert: " + tiempoInsert + " ms");
        System.out.println("Traversal: " + tiempoTraversal + " ms");
    }
}