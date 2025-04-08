#include <Shader.h>

Shader::Shader(std::string vertexFile, std::string fragmentFile) {
	std::string data;

	//Vertex
	readFromFile(vertexFile, data);
	const char* vertexShaderSource = data.c_str();
	unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);

	//Fragment
	readFromFile(fragmentFile, data);
	const char* fragmentShaderSource = data.c_str();
	unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	//Shader Program 
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	//Check for compile time erorrs for shader program
	int  success;
	char infoLog[512];
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADERPRGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
		abort();
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}
Shader::Shader(std::string vertexFile) {
	std::string data;

	//Vertex
	readFromFile(vertexFile, data);
	const char* vertexShaderSource = data.c_str();
	unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);


	//Shader Program 
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glLinkProgram(ID);

	//Check for compile time erorrs for shader program
	int  success;
	char infoLog[512];
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADERPRGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
		abort();
	}

	glDeleteShader(vertexShader);
}
// use/activate the shader
void Shader::use() {
	glUseProgram(ID);
}
// utility uniform functions
void Shader::setBool(const std::string& name, bool value) const{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec2(const std::string &name, glm::vec2 values) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), values.x, values.y);
}
void Shader::setVec3(const std::string& name, std::vector<float> values) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), values[0], values[1], values[2]);
}
void Shader::setVec3(const std::string& name, glm::vec3 values) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), values[0], values[1], values[2]);
}
void Shader::setMat4(const std::string& name, glm::mat4 transformMatrix)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(transformMatrix));
}



//Read from file
void readFromFile(std::string filename, std::string& data) {

	std::ifstream shaderFile;

	// ensure ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		shaderFile.open(filename);

		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();

		data = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << std::endl;
		std::cout << e.what() << "\n";
		std::cout << filename << "\n";
	}
}

//Compile Shaders
unsigned int& compileShader(GLenum shaderType, const char* shaderSource) {
	unsigned int shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);

	//Check for compile time errors
	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		switch (shaderType) {
		case GL_VERTEX_SHADER:
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			break;
		case GL_FRAGMENT_SHADER:
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			break;
		default:
			std::cout << "ERROR::SHADER::OTHER::COMPILATION_FAILED\n" << infoLog << std::endl;
			break;
		}

		abort();
	}
	return shader;
}