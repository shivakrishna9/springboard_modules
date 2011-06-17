<h2>Thermometer template file output</h2>
<?php
 /*
  Relevant available properties:
  $goal->fundraiser_goal_target - unformatted integer, goal target
  $goal->goal_progress - unformatted integer, progress towards goal
  $goal->goal_percentage - rounded percentage of current goal reached
  
  $goal object also contains full node properties for the webform node

  */
?>
<div class="goal-thermometer">
<div class="thermometer" style="float:left;width:20px;height:100px;border:1px solid black">
  <div class="reading" style="height:<?php print $goal->goal_percentage ?>px;background-color:#0F1;margin-top: <?php print 100 - $goal->goal_percentage?>px">

   &nbsp;
  </div>
</div>
<span class="goal-label"> <?php print $goal->fundraiser_goal_target ?></span> 
</div>
