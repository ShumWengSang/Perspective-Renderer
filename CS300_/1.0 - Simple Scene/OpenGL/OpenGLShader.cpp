//
// Created by roland on 9/24/20.
//
#include "../stdafx.h"
#include "OpenGLShader.h"
///////////////////
static GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

///////////////////
static GLuint LoadPipeline(const char * vertex_file_path,const char * fragment_file_path, GLuint * programIDs );

// Load shaders where multiple shader files == one complete shader
// Show how code can be reused across shaders
static GLuint LoadMultiShaders(const char *vertex_file_path, const char *fragment_file_path,
                               const char *geom_file_path, Primitive_Enum  out_primitive_type );


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path)
{

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open()){
        std::string Line;
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }else{
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line;
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;


    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }


    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , nullptr);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }


    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }


    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

GLuint LoadPipeline(const char * vertex_file_path,const char * fragment_file_path, GLuint * programIDs )
{
    // Hardcoding the values to 2

    // Vertex Shader + Program
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open()){
        std::string Line;
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }else{
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();
        return 0;
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    programIDs[0] = glCreateProgram();
    glAttachShader(programIDs[0], VertexShaderID);

    // IMP: Do this BEFORE linking the progam
    glProgramParameteri(programIDs[0],
                        GL_PROGRAM_SEPARABLE,
                        GL_TRUE );

    glLinkProgram(programIDs[0]);

    // Check the program
    glGetProgramiv(programIDs[0], GL_LINK_STATUS, &Result);
    glGetProgramiv(programIDs[0], GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(programIDs[0], InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }





    // Fragment Shader + Program

    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line;
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    Result = GL_FALSE;
    InfoLogLength = 0;

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , nullptr);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    programIDs[1] = glCreateProgram();
    glAttachShader(programIDs[1], FragmentShaderID);

    // IMP: Do this BEFORE linking the progam
    glProgramParameteri(programIDs[1],
                        GL_PROGRAM_SEPARABLE,
                        GL_TRUE );

    glLinkProgram(programIDs[1]);

    GLuint pipeLineID;
    glGenProgramPipelines( 1, &pipeLineID );

    glUseProgramStages( pipeLineID, GL_VERTEX_SHADER_BIT, programIDs[0]);
    glUseProgramStages( pipeLineID, GL_FRAGMENT_SHADER_BIT, programIDs[1]);

    return pipeLineID;
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

GLuint LoadMultiShaders(const char *vertex_file_path, const char *fragment_file_path,
                        const char *geom_file_path,
                        Primitive_Enum out_primitive_type)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;

    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open())
    {
        std::string Line;
        while (getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    } else
    {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n",
               vertex_file_path);
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open())
    {
        std::string Line;
        while (getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }


    // array of char * to hold the strings
    std::vector<const char *> shaderStrings;
    shaderStrings.clear();

    // First string
    std::string geometryShaderHeader("#version 450 core\n"
                                     "\n"
                                     "const int maxVerts = 4;");
    shaderStrings.push_back(geometryShaderHeader.c_str());

    // Second string
    std::string geometryInOut = "// Type of primitive input\n  // from VS (CPU)\n layout (points) in;";

    std::string temp;
    switch (out_primitive_type)
    {
        case TriangleStrip:
            temp = "triangle_strip";
            break;

        case LineStrip:
            temp = "line_strip";
            break;

        case Points:
            temp = "points";
            break;
    };

    geometryInOut += "layout (" + temp + ", max_vertices = maxVerts) out;\n";
    shaderStrings.push_back(geometryInOut.c_str());

    // Third string
    // Read the Geometry Shader from the file
    std::string GeometryShaderCode;
    std::ifstream GeometryShaderStream(geom_file_path, std::ios::in);
    if (GeometryShaderStream.is_open())
    {
        std::string Line;
        while (getline(GeometryShaderStream, Line))
            GeometryShaderCode += "\n" + Line;
        GeometryShaderStream.close();
    }

    shaderStrings.push_back(GeometryShaderCode.c_str());
    //////////////////////////////////////////////////////////


    GLint Result = GL_FALSE;
    int InfoLogLength;


    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const *VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> VertexShaderErrorMessage(static_cast<unsigned long>(InfoLogLength + 1));
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }


    // Compile Geometry Shader
    printf("Compiling shader : %s\n", geom_file_path);
    char const **GeometrySourcePointer = shaderStrings.data(); //GeometryShaderCode.c_str();
    glShaderSource(GeometryShaderID, 3, GeometrySourcePointer, nullptr);
    glCompileShader(GeometryShaderID);

    // Check Fragment Shader
    glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> GeometryShaderErrorMessage((unsigned long) (InfoLogLength + 1));
        glGetShaderInfoLog(GeometryShaderID, InfoLogLength, nullptr, &GeometryShaderErrorMessage[0]);
        printf("%s\n", &GeometryShaderErrorMessage[0]);
    }


    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const *FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> FragmentShaderErrorMessage(static_cast<unsigned long>(InfoLogLength + 1));
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, GeometryShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> ProgramErrorMessage(static_cast<unsigned long>(InfoLogLength + 1));
        glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }


    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, GeometryShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(GeometryShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

void OpenGLShader::Bind() const {
    glUseProgram(RendererID);
}

void OpenGLShader::Unbind() const {
    glUseProgram(0);
}

void OpenGLShader::SetInt(const std::string &name, int value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    glUniform1i(location, value);
}

void OpenGLShader::SetFloat(const std::string &name, float value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    glUniform1f(location, value);
}

void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3 &value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::SetFloat4(const std::string &name, const glm::vec4 &value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::SetMat4(const std::string &name, const glm::mat4 &value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void OpenGLShader::SetMat3(const std::string &name, const glm::mat3 &value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

const std::string &OpenGLShader::GetName() const {
    return Name;
}

std::string OpenGLShader::ReadFile(const std::string &fileName) {

    std::cout << "Reading file " << fileName << std::endl;
    std::string result;
    std::ifstream in(fileName, std::ios::in | std::ios::binary);
    if(in)
    {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();

        // Resize to size, place it all in.
        if(size != -1)
        {
            result.resize(size);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], size);
        }
        else
        {
            // ERROR READING FILE
            DEBUG_BREAKPOINT();
        }
    }
    else
    {
        // ERROR OPENING FILE
        DEBUG_BREAKPOINT();
    }
    return result;
}

GLuint OpenGLShader::Compile(const std::string &shaderContent, GLenum shaderType) {
    // Create the shaders
    GLuint shaderID = glCreateShader(shaderType);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Shader
    printf("Compiling shader %s: \n", shaderType == GL_VERTEX_SHADER ? "GL_VERTEX_SHADER" : "GL_FRAGMENT_SHADER");
    const char * content = shaderContent.data();
    glShaderSource(shaderID, 1, &content , nullptr);
    glCompileShader(shaderID);

    // Check Shader
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(shaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }
    return shaderID;
}

void OpenGLShader::LinkProgram(GLuint shaderID) {
    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Link the program
    printf("Linking program\n");

    glAttachShader(RendererID, shaderID);
    glLinkProgram(RendererID);

    // Check the program
    glGetProgramiv(RendererID, GL_LINK_STATUS, &Result);
    glGetProgramiv(RendererID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(RendererID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(RendererID, shaderID);

    glDeleteShader(shaderID);
}

OpenGLShader::OpenGLShader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc) : Name(name){

    // Read the vertex program
    std::string shaderContent = ReadFile(vertexSrc);
    GLuint shaderID = Compile(shaderContent, GL_VERTEX_SHADER);
    RendererID = glCreateProgram(); // Create ProgramID BEFORE linking
    LinkProgram(shaderID);

    shaderContent = ReadFile(fragmentSrc);
    shaderID = Compile(shaderContent, GL_FRAGMENT_SHADER);
    LinkProgram(shaderID);
}
#ifdef _WIN32
char sep = '\\'
#elif linux
char sep = '/';
#endif
OpenGLShader::OpenGLShader(const std::string &filepath) :  OpenGLShader(filepath.substr(
                                                                            filepath.rfind(sep, filepath.length()) + 1,
                                                                            filepath.length() -
                                                                            filepath.rfind(sep, filepath.length())),
                                                                        filepath + ".vertex",
                                                                        filepath + ".fragment"){
    // Divert to main constructor.

}

OpenGLShader::~OpenGLShader() {
    glDeleteProgram(RendererID);
}

