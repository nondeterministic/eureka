-- Returns true, if the game's winning condition is satisfied, false otherwise.

-- Game is over when Evelyn is brought back to the surface...
function game_won()
   for i=1,(simpl_get_partysize() - 1) do
      if (string.upper(simpl_get_player_name(i)) == "EVELYN") then
	 return simpl_party_is_outdoors()
      end
   end
   
   return false
end
