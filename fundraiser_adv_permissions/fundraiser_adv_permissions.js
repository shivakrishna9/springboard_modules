Drupal.behaviors.permsBehavior = function(){
  $links = $(".tabs.secondary li:first");
  alert('links:' + $links);
  $links.addClass("active");
}
