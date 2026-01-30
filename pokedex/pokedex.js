
const pokemonCount = 151;
var pokedex = {}; // map structure -> {1 : {"name" : "bulbasaur", "img" : url, "type" : ["grass", "poison"], "desc" : "...."}  }

window.onload = async function() {
    getPokemon(1);
}

async function getPokemon(num) {
    let url = "https://pokeapi.co/apiv2/pokemon" + num.toString(); // grab api

    let res = await fetch(url);
    let pokemon = await res.json;
    console.log(pokemon);

}