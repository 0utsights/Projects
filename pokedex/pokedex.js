
const pokemonCount = 151;
var pokedex = {}; // map structure -> {1 : {"name" : "bulbasaur", "img" : url, "type" : ["grass", "poison"], "desc" : "...."}  }

window.onload = function() {
    getPokemon(1);
}

function getPokemon(num) {
    let url = "https://pokeapi.co/apiv2/pokemon" + num.toString(); // grab api

    let res = fetch(url);
    let pokemon = res.json;
    console.log(pokemon);

}