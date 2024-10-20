#version 330

#ifdef VERTEX_SHADER
// doit calculer la position d'un sommet dans le repere projectif
// indice du sommet : gl_VertexID

void main( )
{
    // il faudra renvoyer les coordonnees du ieme sommet (en fonction de la valeur de gl_VertexID)
    //gl_Position= { ... }; // a completer
    vec4 positions[3]= vec4[3]( vec4(0, 1, 0, 1), vec4(-1, 0, 0, 1), vec4(1, 0, 0, 1) );
    gl_Position= positions[gl_VertexID];
    //double x = gl_FragCoord.xy;
}
#endif

#ifdef FRAGMENT_SHADER
// doit calculer la couleur du fragment

void main( )
{
    //vec2 t = mod(gl_FragCoord.xy,(40,40));
    vec3 c = mod(gl_FragCoord.xyz,20);
    c = c /20;
    //vec
//- gl_FragCoord.xy contient les coordonnées du pixel
// sur lequel s'exécute un fragment shader, 
//un modulo ou une division entière devrait 
//permettre de découper l'image en grille...


    gl_FragColor= vec4(c,1); // blanc opaque
    //float t = mod(2,2);


}
#endif