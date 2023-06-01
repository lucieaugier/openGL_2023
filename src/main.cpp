#include <iostream>
#include <vector>
#include "boids/boids.hpp"
#include "boids/field.hpp"
#include "boids/strengths.hpp"
#include "glimac/common.hpp"
#include "glimac/freeflyCamera.hpp"
#include "glimac/sphere_vertices.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/trigonometric.hpp"
#include "img/src/Image.h"
#include "loadingProgram.hpp"
#include "mainCharacter.hpp"
#include "model.hpp"
#include "p6/p6.h"

strengths strengths = {1, 1, 1, 0.06f, glm::length(glm::vec3(0.003f, 0.003f, 0.003f))};

int main()
{
    // creation du contexte p6
    auto ctx = p6::Context{{800, 600, "Boids"}};
    ctx.maximize_window();

    MainCharacter mainCharacter = MainCharacter();

    ///////////// GESTION DE LA CAMERA ///////////////

    FreeflyCamera camera;
    glm::mat4     viewCamera = camera.getViewMatrix();

    // bool leftRot  = false;
    // bool rightRot = false;
    // bool upRot    = false;
    // bool downRot  = false;

    bool leftMove  = false;
    bool rightMove = false;
    bool upMove    = false;
    bool downMove  = false;

    // ctx.mouse_scrolled = [&](p6::MouseScroll scroll) {
    //     camera.zoom(-scroll.dy);
    // };

    /////////////// GESTION DES SHADERS ///////////////

    Program program;

    ///////////// GESTION DES TEXTURES ///////////////

    img::Image nemoMap          = p6::load_image_buffer("assets/textures/clownfish.png");
    img::Image tortueMap        = p6::load_image_buffer("assets/textures/rock_wall_diff_2k.jpg");
    img::Image environnementMap = p6::load_image_buffer("assets/textures/anemone.jpg");
    img::Image anchorMap        = p6::load_image_buffer("assets/textures/Metal041C_1K_Color.jpg");

    program._program.use();

    GLuint texNemo = 0;
    glGenTextures(1, &texNemo);

    glBindTexture(GL_TEXTURE_2D, texNemo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nemoMap.width(), nemoMap.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nemoMap.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint texTortue = 0;
    glGenTextures(1, &texTortue);

    glBindTexture(GL_TEXTURE_2D, texTortue);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tortueMap.width(), tortueMap.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tortueMap.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint texEnvironnement = 0;
    glGenTextures(1, &texEnvironnement);

    glBindTexture(GL_TEXTURE_2D, texEnvironnement);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, environnementMap.width(), environnementMap.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, environnementMap.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint texAnchor = 0;
    glGenTextures(1, &texAnchor);

    glBindTexture(GL_TEXTURE_2D, texAnchor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, anchorMap.width(), anchorMap.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, anchorMap.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    // glBindTexture(GL_TEXTURE_2D, 0);

    ///////////// GESTION DES OBJETS ///////////////

    // chargement des modèles////
    Model nemo = Model();
    nemo.loadModel("nemo.obj");

    Model tortue = Model();
    tortue.loadModel("turtle.obj");

    Model environnement = Model();
    environnement.loadModel("environnement - sans ancre.obj");

    Model anchor = Model();
    anchor.loadModel("anchor.obj");

    glEnable(GL_DEPTH_TEST);

    ////gestion des VBO////

    nemo.createVBO();
    nemo.createVAO();

    tortue.createVBO();
    tortue.createVAO();

    environnement.createVBO();
    environnement.createVAO();

    anchor.createVBO();
    anchor.createVAO();

    ///////////// GESTION DES BOIDS ///////////////
    Field field(50, ctx);

    ctx.update = [&]() {
        std::vector<glm::vec3> positions = field.fieldDraw(ctx);
        field.applyRules(strengths);

        program._program.use();

        ///////////// GESTION DE LA CAMERA ///////////////

        if (rightMove)
        {
            mainCharacter.rotateLeft(-0.5f);
        }
        if (leftMove)
        {
            mainCharacter.rotateLeft(+0.5f);
        }
        if (upMove)
        {
            mainCharacter.moveFront(0.05f);
        }
        if (downMove)
        {
            mainCharacter.moveFront(-0.05f);
        }

        ctx.key_pressed = [&rightMove, &upMove, &leftMove, &downMove](p6::Key key) {
            if (key.logical == "d")
            {
                rightMove = true;
            }
            if (key.logical == "q")
            {
                leftMove = true;
            }
            if (key.logical == "z")
            {
                upMove = true;
            }
            if (key.logical == "s")
            {
                downMove = true;
            }
        };

        ctx.key_released = [&rightMove, &upMove, &leftMove, &downMove](p6::Key key) {
            if (key.logical == "d")
            {
                rightMove = false;
            }
            if (key.logical == "q")
            {
                leftMove = false;
            }
            if (key.logical == "z")
            {
                upMove = false;
            }
            if (key.logical == "s")
            {
                downMove = false;
            }
        };

        ctx.mouse_dragged = [&mainCharacter](const p6::MouseDrag& button) {
            mainCharacter.rotateLeft(button.delta.x * 5);
            mainCharacter.rotateUp(-button.delta.y * 5);
        };

        ctx.mouse_scrolled = [&](p6::MouseScroll scroll) {
            mainCharacter.moveFront(-scroll.dy);
        };

        glm::mat4 viewMatrix = mainCharacter.getViewMatrix();

        ///////////// GESTION DE LA CAMERA ///////////////
        // glm::mat4 viewMatrix = camera.getViewMatrix();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 ProjMatrix   = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.f);
        glm::mat4 MVMatrix     = glm::translate(glm::mat4(1.0), glm::vec3(0., -5., -5.));
        MVMatrix               = viewMatrix * MVMatrix;
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glBindVertexArray(nemo.getVao());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texNemo); // bind txt moon à la place
        glUniform1i(program.uTexture, 0);

        for (size_t i = 0; i < field.getBoids().size(); i++)
        {
            MVMatrix = glm::translate(glm::mat4{1.f}, {0.f, 0.f, 0.f}); // Translation
            MVMatrix = glm::translate(MVMatrix, positions[i]);          // Translation * Rotation * Translation
            MVMatrix = glm::scale(MVMatrix, glm::vec3{1.f});
            MVMatrix = viewMatrix * MVMatrix;
            // glm::mat4 MVMatrixBoids = glm::translate(glm::mat4{1.f}, {1.f, 1.f, -1.f}); // Translation
            // MVMatrixBoids           = glm::translate(MVMatrixBoids, positions[i]);      // Translation * Rotation * Translation
            // MVMatrixBoids           = glm::scale(MVMatrixBoids, glm::vec3{1., 1., 1.}); // Translation * Rotation * Translation * Scale
            // MVMatrixBoids           = MVMatrix * MVMatrixBoids;
            glUniformMatrix4fv(program.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
            glUniformMatrix4fv(program.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
            glUniformMatrix4fv(program.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
            glDrawArrays(GL_TRIANGLES, 0, nemo.getVertices().size());
        };

        // debinder le vbo
        glBindVertexArray(0);

        glBindVertexArray(tortue.getVao());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texTortue); // bind txt tortue à la place
        glUniform1i(program.uTexture, 0);

        MVMatrix = glm::translate(glm::mat4{1.f}, {0.f, 0.f, 0.f});
        MVMatrix = glm::translate(MVMatrix, mainCharacter.getPosition());
        MVMatrix = glm::scale(MVMatrix, glm::vec3{2.f});
        MVMatrix = viewMatrix * MVMatrix;

        glUniformMatrix4fv(program.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(program.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(program.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glDrawArrays(GL_TRIANGLES, 0, tortue.getVertices().size());

        glBindVertexArray(0);

        glBindVertexArray(environnement.getVao());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texEnvironnement); // bind txt environnement à la place
        glUniform1i(program.uTexture, 0);

        MVMatrix = glm::translate(glm::mat4{1.f}, {0.f, 0.f, 0.f});
        MVMatrix = glm::translate(MVMatrix, mainCharacter.getPosition());
        MVMatrix = glm::scale(MVMatrix, glm::vec3{2.f});
        MVMatrix = viewMatrix * MVMatrix;

        glUniformMatrix4fv(program.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(program.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(program.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glDrawArrays(GL_TRIANGLES, 0, environnement.getVertices().size());

        glBindVertexArray(0);

        glBindVertexArray(anchor.getVao());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texAnchor); // bind txt anchor à la place
        glUniform1i(program.uTexture, 0);

        MVMatrix = glm::translate(glm::mat4{1.f}, {0.f, 0.f, 0.f});
        MVMatrix = glm::translate(MVMatrix, mainCharacter.getPosition());
        MVMatrix = glm::scale(MVMatrix, glm::vec3{2.f});
        MVMatrix = viewMatrix * MVMatrix;

        glUniformMatrix4fv(program.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(program.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(program.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glDrawArrays(GL_TRIANGLES, 0, anchor.getVertices().size());

        glBindVertexArray(0);

        camera.followCharacter(mainCharacter.getPosition());
    };

    ctx.start();

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texNemo);
    glDeleteTextures(1, &texTortue);
    glDeleteTextures(1, &texEnvironnement);
    glDeleteTextures(1, &texAnchor);
}
