#include "Object.h"
#include "MtlReader.h"
#include "iostream"

void Object::initialize(string filePath, Shader* shader, glm::vec3 position, glm::vec3 scale, float angle, glm::vec3 axis)
{
	this->position = position;
	this->scale = scale;
	this->angle = angle;
	this->axis = axis;
	this->shader = shader;
	this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->translation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->scale = scale;

	std::cout << "---\n";
	std::cout << "Filepath com o qual o initialize foi chamado: " << filePath << std::endl;
	std::cout << "---\n";

	//Código TGB: a partir do path do arquivo Obj, obtém o arquivo mTL correspondente:
	MtlReader mtlReader;
	this->mtlContent = mtlReader.read(getMTLFilePath(filePath));
	// Imprime todos os MaterialProps em matProps
	for (const auto& material : this->mtlContent.matProps) {
		std::cout << "ka: (" << material.ka.x << ", " << material.ka.y << ", " << material.ka.z << ")\n";
		std::cout << "kd: (" << material.kd.x << ", " << material.kd.y << ", " << material.kd.z << ")\n";
		std::cout << "ks: (" << material.ks.x << ", " << material.ks.y << ", " << material.ks.z << ")\n";
		std::cout << "---\n";
	}
	// Imprime todos os nomes de textura em texNames
	for (const auto& texName : this->mtlContent.texNames) {
		std::cout << "Texture name: " << texName << "\n";
	}


	loadObj(filePath);
}

void Object::update()
{
	glm::mat4 model = glm::mat4(1); //matriz identidade
	//model = glm::translate(model, position);
	//model = glm::rotate(model, glm::radians(angle), axis);

	//Código TGB: aplicação das rotações e translação

	model = glm::translate(model, position + translation);
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, scale);
	shader->setMat4("model", glm::value_ptr(model));
}

void Object::draw()
{
	for (int i = 0; i < grupos.size(); i++)
	{
		grupos[i].draw();
	}
}

void Object::loadObj(string filePath)
{
	//string texNames[] = { "../../3D_models/Pokemon/textures/PikachuMouthDh.png",
	//	"../../3D_models/Pokemon/textures/PikachuDh.png",
	//	"../../3D_models/Pokemon/textures/PikachuHohoDh.png",
	//	"../../3D_models/Pokemon/textures/PikachuEyeDh.png",
	//	"../../3D_models/Pokemon/textures/PikachuDh.png" };
	//string texNames[] = this->mtlContent.texNames;

	//Código TGB: pega os nomes dos arquivos de textura e atribui a um array:
	std::vector<std::string>& texNames = this->mtlContent.texNames;
	/*std::string* texNamesArray = new std::string[texNames.size()];
	std::copy(texNames.begin(), texNames.end(), texNamesArray);*/
	//Código TGB pega as propriedades do material de uma determinada textura e salva num array:
	std::vector<MaterialProps>& matProps = this->mtlContent.matProps;
	/*MaterialProps* matPropsArray = new MaterialProps[matProps.size()];
	std::copy(matProps.begin(), matProps.end(), matPropsArray);*/

	int iTexture = 0;

	ifstream inputFile;
	inputFile.open(filePath);
	vector <GLfloat> vertbuffer;

	vector <glm::vec3> vertices, colors;
	vector <glm::vec3> normals;
	vector <glm::vec2> texCoord;

	bool initializeGroup = true;

	if (inputFile.is_open())
	{
		char line[100];
		string sline;


		while (!inputFile.eof())
		{
			inputFile.getline(line, 100);
			sline = line;

			string word;
			istringstream ssline(sline);

			ssline >> word;

			if (initializeGroup)
			{

			}

			if (word == "v" || inputFile.eof())
			{

				if (initializeGroup)
				{


					if (vertbuffer.size())
					{
						initializeGroup = false;
						Mesh m;
						int nVertices;
						GLuint VAO = generateVAO(vertbuffer, nVertices);
						//Código TGB: gera as texturas a partir do que foi lido do arquivo .mtl
						GLuint texID = generateTexture(texNames[iTexture]);
						//Código TGB: Seta os parâmetros de propriedades do material:
						setMaterialPropertiesOnShader(shader, matProps[iTexture]);
						iTexture++;
						m.initialize(VAO, nVertices, shader, texID);
						grupos.push_back(m);

						//vertices.clear();
						//colors.clear();
						//normals.clear();
						//texCoord.clear();
						vertbuffer.clear();
					}

				}
				glm::vec3 v, color;
				ssline >> v.x >> v.y >> v.z;
				color.r = 1.0; color.g = 0.0; color.b = 0.0;
				vertices.push_back(v);
				colors.push_back(color);
			}
			if (word == "vt")
			{
				glm::vec2 vt;
				ssline >> vt.s >> vt.t;
				texCoord.push_back(vt);
			}
			if (word == "vn")
			{
				glm::vec3 vn;
				ssline >> vn.x >> vn.y >> vn.z;
				normals.push_back(vn);
			}

			if (word == "g") //início de um novo grupo
			{
				initializeGroup = true;
			}
			if (word == "f")
			{
				string tokens[3];
				for (int i = 0; i < 3; i++)
				{
					ssline >> tokens[i];
					int pos = tokens[i].find("/");
					string token = tokens[i].substr(0, pos);
					int index = atoi(token.c_str()) - 1;

					vertbuffer.push_back(vertices[index].x);
					vertbuffer.push_back(vertices[index].y);
					vertbuffer.push_back(vertices[index].z);
					vertbuffer.push_back(colors[index].r);
					vertbuffer.push_back(colors[index].g);
					vertbuffer.push_back(colors[index].b);

					tokens[i] = tokens[i].substr(pos + 1);
					pos = tokens[i].find("/");
					token = tokens[i].substr(0, pos);
					int indexT = atoi(token.c_str()) - 1;

					vertbuffer.push_back(texCoord[indexT].s);
					vertbuffer.push_back(texCoord[indexT].t);

					tokens[i] = tokens[i].substr(pos + 1);
					token = tokens[i].substr(0, pos);
					int indexN = atoi(token.c_str()) - 1;

					vertbuffer.push_back(normals[indexN].x);
					vertbuffer.push_back(normals[indexN].y);
					vertbuffer.push_back(normals[indexN].z);

				}

			}

		}

		inputFile.close();
	}
	else
	{
		cout << "Não foi possivel abrir o arquivo " << filePath << endl;
	}

}

GLuint Object::generateVAO(vector<GLfloat> vertbuffer, int& nVertices)
{
	nVertices = vertbuffer.size() / 11;

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vertbuffer.size() * sizeof(GLfloat), vertbuffer.data(), GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo coordenadas de textura (s, t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//Atributo vetor normal (x, y e z)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

int Object::generateTexture(string filePath)
{
	GLuint texID;

	std::cout << "---\n";
	std::cout << " Caminho para o arquivo da textura: ", filePath;

	// Gera o identificador da textura na memória
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
				data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

void Object::setMaterialPropertiesOnShader(Shader* shader, const MaterialProps& matProps) {
	shader->setVec3("ka", matProps.ka[0], matProps.ka[1], matProps.ka[2]);
	shader->setVec3("kd", matProps.kd[0], matProps.kd[1], matProps.kd[2]);
	shader->setVec3("ks", matProps.ks[0], matProps.ks[1], matProps.ks[2]);
	//shader->setFloat("ka", matProps.ka[0]);
 //   shader->setFloat("kd", matProps.kd[0]);
	//shader->setFloat("ks", matProps.ks[0]);
}


std::string Object::getMTLFilePath(const std::string& caminhoObj) {
	std::string mtlPath;

	std::size_t pos = caminhoObj.find_last_of(".");
	if (pos != std::string::npos) {
		mtlPath = caminhoObj.substr(0, pos) + ".mtl";
	}
	else {
		throw std::runtime_error("Erro: Não foi possível encontrar a extensão do arquivo.");
	}

	return mtlPath;
}