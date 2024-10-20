#include "terrain.hpp"
#include "controller.hpp"
#include "player.hpp"
#include "texture.h"

#include "mat.h"
#include "wavefront.h"
#include "orbiter.h"
#include "draw.h"
#include "app.h"

class Play : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    Play() : App(1024, 640),
             controller1_(SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT),
             controller2_('z', 's', 'q', 'd'),
             terrain_(Point(-20.f, -20.f, 0.f), Point(20.f, 20.f, 0.f)),
             quad_(GL_TRIANGLES)

    {
    }

    int init()
    {
        vehicule1_ = read_mesh(smart_path("../assets/mmachine.obj"));
        vehicule1_.default_color(Color(1.0f, 0.f, 0.f));
        vehicule2_ = read_mesh(smart_path("../assets/mmachine.obj"));
        vehicule2_.default_color(Color(0.0f, 0.f, 1.f));

        joueur1_.set_terrain(&terrain_);
        joueur1_.set_controller(&controller1_);
        joueur1_.spawn_at(Point(0, 0, 0), Vector(0, 1, 0));
        joueur1_.activate();

        joueur2_.set_terrain(&terrain_);
        joueur2_.set_controller(&controller2_);
        joueur2_.spawn_at(Point(1, 1, 0), Vector(0, 1, 0));
        joueur2_.activate();
        m_camera.lookat(Point(-10.f, -10.f, -10.f), Point(10.f, 10.f, 10.f));
        m_camera.rotation(180.0, 0);
        mode_cool = false;
        cam = Point(0, 0, 5.0f);
        // quad_.default_color(Color(1.0f, 0.f, 0.f));

        std::cout
            << quad_.default_color().r
            << " "
            << quad_.default_color().g
            << " "
            << quad_.default_color().b
            << std::endl;

        quad_.texcoord(0.f, 0.f);
        quad_.vertex(-0.95f, -0.95f, 0.f);

        quad_.texcoord(1.f, 0.f);
        quad_.vertex(0.95f, -0.95f, 0.f);
        // quad_.color(255.0f, 0.0f, 0.0f, 1.0f);
        quad_.texcoord(1.f, 1.f);
        quad_.vertex(0.95, 0.95f, 0.f);

        quad_.texcoord(0.f, 1.f);
        quad_.vertex(-0.95f, 0.95f, 0.f);

        quad_.triangle(0, 1, 2);
        quad_.triangle(0, 2, 3);
        // quad_.color(255.0f, 0.0f, 0.0f, 1.0f);

        texture_ = read_texture(0, "/tmp/transparent.png");

        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f); // couleur par defaut de la fenetre

        glClearDepth(1.f);       // profondeur par defaut
        glDepthFunc(GL_LESS);    // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST); // activer le ztest

        return 0; // ras, pas d'erreur
    }

    // destruction des objets de l'application
    int quit()
    {
        quad_.release();

        vehicule1_.release();
        vehicule2_.release();
        return 0;
    }

    // dessiner une nouvelle image
    int render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawParam param;
        // deplace la camera
        int mx, my;
        unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);
        if (mb & SDL_BUTTON(1)) // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if (mb & SDL_BUTTON(3)) // le bouton droit est enfonce
            m_camera.move(mx);
        else if (mb & SDL_BUTTON(2)) // le bouton du milieu est enfonce
            m_camera.translation((float)mx / (float)window_width(), (float)my / (float)window_height());

        Transform T = Identity();
        Point j1_world;
        Point j2_world;
        Point between;
        if (mode_cool == true)
        {
            j1_world = T(joueur1_.position_);
            j2_world = T(joueur2_.position_);
            between = Point((j1_world.x + j2_world.x) / 2, (j1_world.y + j2_world.y) / 2, (j1_world.z + j2_world.z) / 2);
            Point cam1(cam.x, cam.y, 0.0f);
            float hauteur = 10.f;
            if (distance(cam1, between) > hauteur)
            {

                cam = cam + normalize(between - cam1) * (distance(cam1, between) - hauteur) * (delta_time() / 1000);

                std::cout << "delta_time" << delta_time() << std::endl;

                std::cout << "on ralloge" << distance(cam1, between) << std::endl;
            }
        }
        else
        {
            j1_world = T(joueur1_.position_) * 1.5;
            j2_world = T(joueur2_.position_);
            between = Point((j1_world.x + j2_world.x) / 2, (j1_world.y + j2_world.y) / 2, (j1_world.z + j2_world.z) / 2);
            cam = Point(between.x, between.y, 50.0);
        }

        Transform view = Lookat(cam, between, Vector(0, 1, 1));

        Transform projection(Perspective(45, (float)window_width() / (float)window_height(), 0.1, 139));
        Point x = T(joueur1_.position_);
        x = view(x);
        x = projection(x);
        Point limite_min_j1; // bas gauche
        Point limite_max_j1; // haut droit
        Point limite_min_j2;
        Point limite_max_j2;

        vehicule1_.bounds(limite_min_j1, limite_max_j1);
        vehicule2_.bounds(limite_min_j2, limite_max_j2);

        Point v1_BG((limite_min_j1 / 2) + joueur1_.position_);
        v1_BG = T(v1_BG);
        limite_min_j1 = v1_BG;
        limite_min_j1 = view(limite_min_j1);
        limite_min_j1 = projection(limite_min_j1);

        Point v1_HD((limite_max_j1 / 2) + joueur1_.position_);
        v1_HD = T(v1_HD);
        limite_max_j1 = v1_HD;
        limite_max_j1 = view(limite_max_j1);
        limite_max_j1 = projection(limite_max_j1);

        Point v2_BG((limite_min_j2 / 2) + joueur2_.position_);
        v2_BG = T(v2_BG);
        limite_min_j2 = v2_BG;
        limite_min_j2 = view(limite_min_j2);
        limite_min_j2 = projection(limite_min_j2);

        Point v2_HD((limite_max_j2 / 2) + joueur2_.position_);
        v2_HD = T(v2_HD);
        limite_max_j2 = v2_HD;
        limite_max_j2 = view(limite_max_j2);
        limite_max_j2 = projection(limite_max_j2);
        if (mode_cool == false)
        {
            if ((limite_min_j1.x < -1 && limite_max_j1.x < -1) || (limite_min_j1.x > 1 && limite_max_j1.x > 1) || (limite_min_j1.y > 1 && limite_max_j1.y > 1) || (limite_min_j1.y < -1 && limite_max_j1.y < -1))
            {

                std::cout << "voiture bleu :"
                          << "gagne" << std::endl;

                quad_.default_color(Color(0.0f, 0.f, 1.f));

                param.texture(texture_);
                param.draw(quad_);
            }
            if ((limite_min_j2.x < -1 && limite_max_j2.x < -1) || (limite_min_j2.x > 1 && limite_max_j2.x > 1) || (limite_min_j2.y > 1 && limite_max_j2.y > 1) || (limite_min_j2.y < -1 && limite_max_j2.y < -1))
            {

                std::cout << "voiture rouge :"
                          << "gagne" << std::endl;

                quad_.default_color(Color(1.0f, 0.f, 0.f));

                param.texture(texture_);
                param.draw(quad_);
            }
        }

        terrain_.draw(view, projection);
        Transform player1_pos = joueur1_.transform();
        draw(vehicule1_, player1_pos, view, projection);
        Transform player2_pos = joueur2_.transform();
        draw(vehicule2_, player2_pos, view, projection);

        //   reset
        if (key_state('r'))
        {
            joueur1_.spawn_at(Point(0, 0, 0), Vector(0, 1, 0));
            joueur1_.activate();
            joueur2_.spawn_at(Point(1, 0, 0), Vector(0, 1, 0));
            joueur2_.activate();
        }
        if (key_state('c'))
        {
            mode_cool = !mode_cool;
            joueur1_.spawn_at(Point(0, 0, 0), Vector(0, 1, 0));
            joueur1_.activate();
            joueur2_.spawn_at(Point(1, 0, 0), Vector(0, 1, 0));
            joueur2_.activate();
            cam = Point(0, 0, 5.0f);
        }

        return 1;
    }

protected:
    Mesh vehicule1_;
    Mesh vehicule2_;
    Player joueur1_;
    Player joueur2_;
    KeyboardController controller1_;
    KeyboardController controller2_;
    bool mode_cool;
    FlatTerrain terrain_;

    Orbiter m_camera;

    Mesh quad_;
    GLuint texture_;

    Point cam;
};

int main(int argc, char **argv)
{
    Play app;
    app.run();

    return 0;
}
