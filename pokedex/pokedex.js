
const pokemonCount = 151;
var pokedex = {}; // map structure -> {1 : {"name" : "bulbasaur", "img" : url, "type" : ["grass", "poison"], "desc" : "...."}  }

window.onload = async function() {
    // getPokemon(1);
    for (let i = 0; i <= pokemonCount; i++) {
        await getPokemon(i);
        //<div id="1">class = pokemon name</div>
        let pokemon = document.createElement("div");
        pokemon.id = i;
        pokemon.innerText = i.toString() + ". " + pokedex[i]["name"].toUpperCase(); // for pokedex name
        pokemon.classList.add("pokemon-name");
        document.GetElementById("pokemon-list").append(pokemon);
    }

    console.log(pokedex);
}

async function getPokemon(num) {
    let url = "https://pokeapi.co/api/v2/pokemon/" + num.toString(); // grab api

    let res = await fetch(url);
    let pokemon = await res.json();
    // console.log(pokemon)

    let pokemonName = pokemon["name"];
    let pokemonType = pokemon["types"];
    let pokemonImg = pokemon["sprites"]["front_default"]; // using later in a different project for ascii image with pokemon sprites in terminal

    res = await fetch(pokemon['species']['url']);
    let pokemonDesc = await res.json();

    // console.log(pokemonDesc);
    pokemonDesc = pokemonDesc["flavor_text_entries"][9]["flavor_text"]

    pokedex[num] = {"name" : pokemonName, "img" : pokemonImg, "types" : pokemonType, "desc" : pokemonDesc}
}