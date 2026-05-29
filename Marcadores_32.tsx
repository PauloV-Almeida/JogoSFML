<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.11.0" name="Marcadores_32" tilewidth="32" tileheight="32" tilecount="7" columns="7">
 <image source="assets/tileset_marcadores_32.png" width="224" height="32"/>
 <tile id="0" type="Chao">
  <properties>
   <property name="valor" type="int" value="1"/>
   <property name="solido" type="bool" value="true"/>
   <property name="desenhar" type="bool" value="false"/>
  </properties>
 </tile>
 <tile id="1" type="PlataformaGelo">
  <properties>
   <property name="valor" type="int" value="2"/>
   <property name="solido" type="bool" value="true"/>
   <property name="escorregadio" type="bool" value="true"/>
   <property name="desenhar" type="bool" value="true"/>
  </properties>
 </tile>
 <tile id="2" type="SpawnPlayer">
  <properties>
   <property name="valor" type="int" value="3"/>
  </properties>
 </tile>
 <tile id="3" type="EspinhoVenenoso">
  <properties>
   <property name="valor" type="int" value="4"/>
   <property name="dano" type="int" value="10"/>
   <property name="venenoTempo" type="float" value="2"/>
  </properties>
 </tile>
 <tile id="4" type="Andarilho">
  <properties>
   <property name="valor" type="int" value="5"/>
  </properties>
 </tile>
 <tile id="5" type="Valkiria">
  <properties>
   <property name="valor" type="int" value="6"/>
  </properties>
 </tile>
 <tile id="6" type="ProximaFase">
  <properties>
   <property name="valor" type="int" value="7"/>
   <property name="destino" value="mapa2"/>
  </properties>
 </tile>
</tileset>
