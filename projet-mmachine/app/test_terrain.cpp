#include "terrain.hpp"
#include "controller.hpp"
#include "player.hpp"
#include "obstacle.hpp"
#include "collider.hpp"

#include "mat.h"
#include "wavefront.h"
#include "orbiter.h"
#include "draw.h"
#include "app.h"
#include "texture.h"

// utilitaire. creation d'une grille / repere.
Mesh make_grid()
{
    Mesh grid = Mesh(GL_LINES);

    for (int x = 0; x < 10; x++)
    {
        float px = (float)x - 5.f + 0.5f;
        grid.vertex(Point(px, 0, -4.5f));
        grid.vertex(Point(px, 0, 4.5f));
    }

    for (int z = 0; z < 10; z++)
    {
        float pz = (float)z - 5.f + 0.5f;
        grid.vertex(Point(-4.5f, 0, pz));
        grid.vertex(Point(4.5f, 0, pz));
    }

    return grid;
}

class Play : public App
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    Play() : App(1024, 640),
             controller1_(SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT),
             controller2_('z', 's', 'q', 'd'),
             controller1_reverse(SDLK_LEFT, SDLK_RIGHT, SDLK_DOWN, SDLK_UP),
             controller2_reverse('q', 'd', 's', 'z'),
             quad_(GL_TRIANGLES),
             terrain_(Point(-20.f, -20.f, 0.f), Point(20.f, 20.f, 20.f), "projet-mmachine/assets/circuit_simple.png")
    {
    }

    void HandleCollision()
    {
        for (int i = 0; i < tabPlayer.size(); i++)
        {
            for (int j = 0; j < tabCollider.size(); j++)
            {
                tabPlayer[i]->collider_.center = tabPlayer[i]->position_;
                if (&tabPlayer[i]->collider_ != tabCollider[j])
                {
                    if (tabPlayer[i]->collider_.collision(*tabCollider[j]))
                    {
                        if (tabCollider[j]->type == PLAYER)
                        {
                            /* 
                            for a collision between 2 vehicles, we could use a formula along the lines of 
                            playerSpeed = playerSpeed - dot(playerspeed, distance(player, otherplayer) * distance(player, otherplayer))   
                            */
                            tabPlayer[i]->speed_ = -tabPlayer[i]->speed_;
                        }
                        else if (tabCollider[j]->type == BUISSON)
                        {
                            tabPlayer[i]->speed_ = tabPlayer[i]->speed_ * 0.9;
                        }
                        else if (tabCollider[j]->type == HUILE && i == 0)
                        {
                            tabPlayer[i]->set_controller(&controller1_reverse);
                        }
                        else if (tabCollider[j]->type == HUILE && i == 1)
                        {
                            tabPlayer[i]->set_controller(&controller2_reverse);
                        }
                        else if (tabCollider[j]->type == CAISSE)
                        {
                            tabPlayer[i]->speed_ = -tabPlayer[i]->speed_ * 0.5;
                        }
                    }
                    else
                    {
                        if (i == 0)
                        {
                            tabPlayer[i]->set_controller(&controller1_);
                        }
                        else
                        {
                            tabPlayer[i]->set_controller(&controller2_);
                        }
                        tabPlayer[i]->acceleration_ = 0.0005f;
                    }
                }
            }
        }
    }

    void HandleAdvancement()
    {
        // compare the % of terrain of both players.
        float p1_rank = terrain_.getCircuitpercentage(joueur1_.position_);
        float p2_rank = terrain_.getCircuitpercentage(joueur2_.position_);

        if (p1_old_p > 99 && p1_rank < p1_old_p)
        {
            std::cout << "player 1 beings a new lap" << std::endl;
            p1_lap += 1;
        }

        // try to increase/decrease the lap count.
        // the idea was to check if the previous value was near to 100, and if the current is above/below, 
        // increase the lap count. Works quite bad so we aren't using it now. 
        // a better solution would be to keep a list of the checkpoints the player visited.
        if (p2_old_p > 99 && p2_rank < p2_old_p)
        {
            std::cout << "player 2 beings a new lap" << std::endl;
            p2_lap += 1;
        }

        if (p1_old_p < 1 && p1_rank > p1_old_p)
            p1_lap -= 1;

        if (p2_old_p < 1 && p2_rank > p2_old_p)
            p2_lap -= 1;

        /*if (p1_lap > p2_lap)
            first = 1;
        else if(p2_lap > p1_lap)
            first = 2;
        else if (p2_lap == p1_lap) {*/
        if (p1_rank > p2_rank)
            first = 1;
        else
            first = 2;
        //}

        // std::cout << "player " << first << " is leading. p1: " << p1_rank << " p2:" << p2_rank << std::endl;

        p1_old_p = p1_rank;
        p2_old_p = p2_rank;
    }

    void HandleCamera()
    {
        DrawParam param;

        Transform T = Identity();
        Point j1_world;
        Point j2_world;
        Point between;
        if (mode_cool == true)
        {
            j1_world = T(joueur1_.position_);
            j2_world = T(joueur2_.position_);
            between = Point((j1_world.x + j2_world.x) / 2, (j1_world.y + j2_world.y) / 2, (j1_world.z + j2_world.z) / 2);
            Point cam1(cam.x, cam.y, 20.0f);
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
        //terrain_.drawCheckpoints(view, projection);
        Transform player1_pos = joueur1_.transform();
        draw(vehicule1_, player1_pos, view, projection);
        Transform player2_pos = joueur2_.transform();
        draw(vehicule2_, player2_pos, view, projection);

        // draw the obstacles
        for (int i = 0; i < terrain_.obstacles.size(); i++)
        {
            Transform obs_pos = Translation(Vector(terrain_.obstacles[i].collider.center))*Scale(0.5,0.5,0.5);
            draw(terrain_.obstacles[i].m_object, obs_pos, view, projection);
        }
    }

    int init()
    {
        vehicule1_ = read_mesh(smart_path("../assets/mmachine.obj"));
        vehicule1_.default_color(Color(1.0f, 0.f, 0.f));
        vehicule2_ = read_mesh(smart_path("../assets/mmachine.obj"));
        vehicule2_.default_color(Color(0.0f, 0.f, 1.f));

        joueur1_.set_terrain(&terrain_);
        joueur1_.set_controller(&controller1_);
        joueur1_.spawn_at(terrain_.p_spawn[0], Vector(-1, 0, 0));
        joueur1_.activate();

        joueur2_.set_terrain(&terrain_);
        joueur2_.set_controller(&controller2_);
        joueur2_.spawn_at(terrain_.p_spawn[1], Vector(-1, 0, 0));
        joueur2_.activate();

        tabPlayer.push_back(&joueur1_);
        tabCollider.push_back(&joueur1_.collider_);

        tabPlayer.push_back(&joueur2_);
        tabCollider.push_back(&joueur2_.collider_);

        for (int i = 0; i < terrain_.obstacles.size(); i++)
        {
            tabCollider.push_back(&terrain_.obstacles[i].collider);
        }

        quad_.texcoord(0.f, 0.f);
        quad_.vertex(-0.30f, -0.10f, 0.f);

        quad_.texcoord(1.f, 0.f);
        quad_.vertex(0.30, -0.10f, 0.f);
        quad_.color(255.0f, 0.0f, 0.0f, 1.0f);
        quad_.texcoord(1.f, 1.f);
        quad_.vertex(0.30, 0.10f, 0.f);
        quad_.color(255.0f, 0.0f, 0.0f, 1.0f);

        quad_.texcoord(0.f, 1.f);
        quad_.vertex(-0.30f, 0.10f, 0.f);

        texture_ = read_texture(0, "/tmp/transparent.png");

        m_camera.lookat(Point(-20.f, -20.f, 0.f), Point(20.f, 20.f, 20.f));

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
        vehicule1_.release();
        vehicule2_.release();
        return 0;
    }

    // dessiner une nouvelle image
    int render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        HandleAdvancement();

        HandleCollision();

        HandleCamera();

        // reset
        if (key_state('r'))
        {
            joueur1_.spawn_at(terrain_.p_spawn[0], Vector(-1, 0, 0));
            joueur1_.activate();

            joueur2_.spawn_at(terrain_.p_spawn[1], Vector(-1, 0, 0));
            joueur2_.activate();
        }

        if (key_state('c'))
        {
            mode_cool = !mode_cool;
            joueur1_.spawn_at(terrain_.p_spawn[0], Vector(-1, 0, 0));
            joueur1_.activate();

            joueur2_.spawn_at(terrain_.p_spawn[1], Vector(-1, 0, 0));
            joueur2_.activate();
            
            cam = terrain_.p_spawn[0] + Point(0, 0, 25.0f);
        }

        // deplace la camera
        int mx, my;
        unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);
        if (mb & SDL_BUTTON(1)) // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if (mb & SDL_BUTTON(3)) // le bouton droit est enfonce
            m_camera.move(mx);
        else if (mb & SDL_BUTTON(2)) // le bouton du milieu est enfonce
            m_camera.translation((float)mx / (float)window_width(), (float)my / (float)window_height());

        return 1;
    }

protected:
    Mesh vehicule1_;
    Mesh vehicule2_;
    Mesh quad_;

    Point cam;

    Player joueur1_;
    Player joueur2_;
    KeyboardController controller1_;
    KeyboardController controller2_;

    KeyboardController controller1_reverse;
    KeyboardController controller2_reverse;

    std::vector<Player *> tabPlayer;
    std::vector<Collider *> tabCollider;

    float p1_old_p, p2_old_p; // position of p1 on the previous frame
    int p1_lap = 0; //number of laps of p1
    int p2_lap = 0; 
    int first; //first player, either 1 or 2
    int second; //second player

    bool mode_cool;

    ImgTerrain terrain_;

    Orbiter m_camera;

    GLuint texture_;
};

int main(int argc, char **argv)
{
    Play app;
    app.run();

    return 0;
}
