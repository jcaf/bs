??             +         ?     ?  *   ?     
               -     6     F  ?  L     ?       ?        ?     ?     ?     ?     ?  
   ?     ?     ?  	   ?  :   ?     "     *  $   0  =   U     ?     ?  "   ?  D   ?  )   $  (  N     w	  -   ?	     ?	     ?	     ?	     ?	     
     
  %  
     :     ?  x   B     ?     ?     ?     ?     ?     ?               (  E   G     ?     ?  6   ?  3   ?          %  ,   <  7   i  /   ?                                                                              	                                   
                               Apikey authentication Are you sure you want to delete this feed? Cancel Datatype Delete permanently Feed API Feed API helper Feeds Feeds are where your monitoring data is stored. The recommended route for creating feeds is to start by creating inputs (see the inputs tab). Once you have inputs you can either log them straight to feeds or if you want you can add various levels of input processing to your inputs to create things like daily average data or to calibrate inputs before storage. You may want to follow the link as a guide for generating your request. Html Id If you want to call any of the following action's when your not logged in, add an apikey to the URL of your request: &apikey=APIKEY. JSON Name No feeds created Public Read & Write: Read only: Tag The feed list view This page To use the json api the request url needs to include .json Updated Value WARNING deleting a feed is permanent returns a list of public feeds made public by the given user. returns feed data returns histogram data returns id of a feed given by name returns kwh consumed in a given power band using histogram data type returns the present value of a given feed Project-Id-Version: emoncms3
Report-Msgid-Bugs-To: 
POT-Creation-Date: 2013-04-28 02:00+0100
PO-Revision-Date: 2013-04-28 02:04+0100
Last-Translator: VIALAT Mickael <contact@planete-domotique.com>
Language-Team: Baptiste Gaultier (Télécom Bretagne) <baptiste.gaultier@telecom-bretagne.eu>
Language: fr_FR
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Poedit-KeywordsList: _;gettext;gettext_noop
X-Poedit-Basepath: .
X-Poedit-SourceCharset: utf-8
X-Generator: Poedit 1.5.5
X-Poedit-SearchPath-0: ../../..
 Authentification APIKey Etes vous sûr de vouloir supprimer ce flux ? Annuler Type de données Supprimer le flux ? API de gestion du flux Aide de l'API Flux Les flux correspondent à l'archivage et au traitement de vos données. Créez d'abort une source de données (Onglet Sources), puis utilisez ensuite les flux pour traiter ces données avant de les archiver. Vous pouvez lire la documentation sur l'API flux pour plus d'information (en Anglais) Html id Si vous voulez appeler ces fonctions sans être connecté, il suffit d'ajouter votre clé API sur l'url : &apikey=APIKEY JSON Nom Aucun flux disponible Public Lecture / Ecriture : Accès en lecture seule :  Tag La liste des flux  Cette page (documentation API) Pour utiliser l'api json, vous devez ajouter .json à la fin de l'url Actualis&eacute; Valeur ATTENTION ! La suppression d'un flux est irréversible Renvoi la liste des flux public pour un utilisateur Les données du flux Données d'histogramme Retourne l'id d'un flux à partir de son nom Renvoi la puissance consommée sous forme d'histogramme Renvoi la valeur courante d'un flux de données 