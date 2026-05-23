#ifndef MODEL_H
#define MODEL_H

#include <GL\GL.h>
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

class Model
{
public:
    vector<Mesh> meshes;
    string directory;

    Model(string const& path)
    {
        loadModel(path);
    }

    // Новый метод для отрисовки
    void Draw(Shader& shader, float tX, float shY, float sZ)
    {
        // матрица для стола
        glm::mat4 matTable = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, tX));

        // матрица для шпиндельной головы
        glm::mat4 matSpindleHead = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, shY, 0.0f));
        // матрица для верхнего кожуха
        glm::mat4 matSaddle = glm::translate(glm::mat4(1.0f), glm::vec3(sZ, 0.0f, 0.0f));

        for (int i = 0; i < meshes.size(); i++) {
            glm::mat4 meshModelMatrix = glm::mat4(1.0f);

            switch (i) {
            case 0: // станина
            case 1: // колонна
                break;
            
           
            case 2:
            case 3:// стол и верхний кожух      
                meshModelMatrix = meshModelMatrix * matSaddle;
                break;

            case 6: // стол
                meshModelMatrix = meshModelMatrix * matSaddle * matTable;
                break;

            case 5: // шпиндельная голова
                meshModelMatrix = meshModelMatrix * matSpindleHead;
                break;

            default:
                break;
            }

            shader.setMat4("model", meshModelMatrix);

            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(meshModelMatrix)));
            shader.setMat3("normalMatrix", normalMatrix);

            meshes[i].Draw();
        }
    }

private:
    void loadModel(string const& path)
    {
        Assimp::Importer importer;
        directory = path.substr(0, path.find_last_of('/'));

        const aiScene* scene = importer.ReadFile(path,
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);

        if (nullptr == scene) {
            fprintf(stderr, "Error ASSIMP:: %s\n", importer.GetErrorString());
            return;
        }

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene)
    {
        for (int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for (int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        vector <Vertex>       vertices;
        vector <unsigned int> indices;

        for (int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vert_buffer;
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vert_buffer.Position = vector;

            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vert_buffer.Normal = vector;
            }
            vertices.push_back(vert_buffer);
        }

        for (int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
        return Mesh(vertices, indices);
    }
};
#endif