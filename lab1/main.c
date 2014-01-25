#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

static const GLfloat triangle[] = {
    0.8f,-0.8f,0.0f,1.0f,
    0.0f,0.8f,0.0f,1.0f,
    -0.8f,-0.8f,0.0f,1.0f,
};

static const GLfloat line[] = {
    0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
};

static const GLfloat points[] = {
    0.75f, 0.75f, 0.0f, 1.0f,
    0.25f, 0.25f, 0.0f, 1.0f,
};

void error(int err, const char *desc) {
    fprintf(stderr, "%s\n", desc);
}

void resize(GLFWwindow *window, int w, int h) {
    glViewport(0, 0, w, h);
}

static int CURRENT_OBJ = 0;

void mousedown(GLFWwindow *window, int button, int action, int mods) {
    if ( action == GLFW_PRESS ) {
        switch ( CURRENT_OBJ ) {
            case 0:
                CURRENT_OBJ = 1;
                break;
            case 1:
                CURRENT_OBJ = 2;
                break;
            case 2:
                CURRENT_OBJ = 0;
                break;
        }
    }
}

char *read_file(const char *filename, int *size) {
    char *contents;
    FILE *f;

    f = fopen(filename, "r");

    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    contents = malloc(*size);
    if (!contents) {
        fclose(f);
        return NULL;
    }
    rewind(f);
    fread(contents, 1, *size, f);
    fclose(f);
    return contents;
}

GLuint compile_shader(GLenum type, const char *src, int len) {
    GLuint shader;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, &len);
    glCompileShader(shader);
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        int len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        char *buf = malloc(len);
        if (!buf) {
            fprintf(stderr, "Out of memory!\n");
            exit(EXIT_FAILURE);
        }
        int alen;
        glGetShaderInfoLog(shader, len, &alen, buf);
        fprintf(stderr, "%s\n", buf);
        free(buf);
        exit(EXIT_FAILURE);
    }
    return shader;
}

int main(int argc, char **argv) {
    glfwSetErrorCallback(error);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow *window = glfwCreateWindow(640, 480, "Lab 1", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, resize);
    glfwSetMouseButtonCallback(window, mousedown);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if ( err != GLEW_OK ) {
        fprintf(stderr, "GLEW: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    fprintf(stderr, "Using version %d.%d\n", major, minor);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    char *frag, *vert;
    int frag_len, vert_len;
    frag = read_file("frag.glsl", &frag_len);
    vert = read_file("vert.glsl", &vert_len);

    if (!frag || !vert) {
        fprintf(stderr, "Failed to read shaders\n");
        exit(EXIT_FAILURE);
    }

    GLuint frag_s, vert_s, prog;

    vert_s = compile_shader(GL_VERTEX_SHADER, vert, vert_len);
    frag_s = compile_shader(GL_FRAGMENT_SHADER, frag, frag_len);

    prog = glCreateProgram();
    glAttachShader(prog, vert_s);
    glAttachShader(prog, frag_s);
    glLinkProgram(prog);

    int status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        fprintf(stderr, "Linking failed!\n");
        exit(EXIT_FAILURE);
    }

    glUseProgram(prog);

    GLuint arrays[3];
    glGenVertexArrays(3, arrays);
    glBindVertexArray(arrays[0]);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(arrays[1]);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(arrays[2]);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(arrays[0]);

    while (!glfwWindowShouldClose(window)) {
        glBindVertexArray(arrays[CURRENT_OBJ]);
        glClear(GL_COLOR_BUFFER_BIT);
        switch ( CURRENT_OBJ ) {
            case 0:
                glDrawArrays(GL_TRIANGLES, 0, 3);
            case 1:
                glDrawArrays(GL_LINES, 0, 2);
            case 2:
                glDrawArrays(GL_POINTS, 0, 2);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
