<?php
 /*
  Relevant available properties:
  $goal->pid - parent id (typically node id) of the goal parent.
  $goal->delta - "basic" for basic goals. Displays the machine name value for advanced goals.
  $goal->target - unformatted integer, goal target
  $goal->display_threshold - the minimum progress value before the goal should be displayed.
  $goal->default_message - additional text to display if no progress has been made toward this goal.
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
