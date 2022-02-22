# Copyright (c) 2022 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License.
Feature: Redefined symbols
  Examples:
    | space_name  |
    | nba         |
    | nba_int_vid |

  Background:
    Given a graph with space named "<space_name>"

  Scenario: Redefined node alias
    When executing query:
      """
      match (v:player)-[:like]->(v) return v.player.name AS name
      """
    Then the result should be, in any order:
      | name |
    When executing query:
      """
      match (v)-[:serve]->(t)<-[:serve]-(v) return t.team.name, v.player.name
      """
    Then the result should be, in any order:
      | t.team.name | v.player.name     |
      | "Mavericks" | "Jason Kidd"      |
      | "Mavericks" | "Jason Kidd"      |
      | "Spurs"     | "Marco Belinelli" |
      | "Spurs"     | "Marco Belinelli" |
      | "Heat"      | "Dwyane Wade"     |
      | "Heat"      | "Dwyane Wade"     |
      | "Suns"      | "Steve Nash"      |
      | "Suns"      | "Steve Nash"      |
      | "Hornets"   | "Marco Belinelli" |
      | "Hornets"   | "Marco Belinelli" |
      | "Cavaliers" | "LeBron James"    |
      | "Cavaliers" | "LeBron James"    |
    When executing query:
      """
      match (v)-[]->(t)<-[]-(v:player) return v.player.name, t.team.name
      """
    Then the result should be, in any order:
      | v.player.name     | t.team.name         |
      | "LeBron James"    | "Cavaliers"         |
      | "LeBron James"    | "Cavaliers"         |
      | "Marco Belinelli" | "Hornets"           |
      | "Marco Belinelli" | "Spurs"             |
      | "Marco Belinelli" | "Hornets"           |
      | "Marco Belinelli" | "Spurs"             |
      | "Tony Parker"     | "Tim Duncan"        |
      | "Tony Parker"     | "LaMarcus Aldridge" |
      | "Tony Parker"     | "Manu Ginobili"     |
      | "Tony Parker"     | "LaMarcus Aldridge" |
      | "Tony Parker"     | "Manu Ginobili"     |
      | "Tony Parker"     | "Tim Duncan"        |
      | "Tim Duncan"      | "Manu Ginobili"     |
      | "Tim Duncan"      | "Tony Parker"       |
      | "Tim Duncan"      | "Manu Ginobili"     |
      | "Tim Duncan"      | "Tony Parker"       |
      | "Manu Ginobili"   | "Tim Duncan"        |
      | "Manu Ginobili"   | "Tim Duncan"        |
      | "Dwyane Wade"     | "Heat"              |
      | "Dwyane Wade"     | "Heat"              |
      | "Steve Nash"      | "Suns"              |
      | "Steve Nash"      | "Suns"              |
      | "Jason Kidd"      | "Mavericks"         |
      | "Jason Kidd"      | "Mavericks"         |
    When executing query:
      """
      match (v)-[]->(t)<-[:serve]-(v) return t.team.name, v.player.name
      """
    Then the result should be, in any order:
      | t.team.name | v.player.name     |
      | "Mavericks" | "Jason Kidd"      |
      | "Mavericks" | "Jason Kidd"      |
      | "Spurs"     | "Marco Belinelli" |
      | "Spurs"     | "Marco Belinelli" |
      | "Heat"      | "Dwyane Wade"     |
      | "Heat"      | "Dwyane Wade"     |
      | "Suns"      | "Steve Nash"      |
      | "Suns"      | "Steve Nash"      |
      | "Hornets"   | "Marco Belinelli" |
      | "Hornets"   | "Marco Belinelli" |
      | "Cavaliers" | "LeBron James"    |
      | "Cavaliers" | "LeBron James"    |

  Scenario: Redefined edge alias
    When executing query:
      """
      MATCH (v:player{name:"abc"})-[e:like]->(v1)-[e:like]->(v2) RETURN *
      """
    Then a SemanticError should be raised at runtime: `e': Redefined alias
